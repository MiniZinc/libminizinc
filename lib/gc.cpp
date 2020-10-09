/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/ast.hh>
#include <minizinc/config.hh>
#include <minizinc/gc.hh>
#include <minizinc/hash.hh>
#include <minizinc/model.hh>
#include <minizinc/timer.hh>

#include <cstring>
#include <vector>

namespace MiniZinc {

GC*& GC::gc() {
#if defined(HAS_DECLSPEC_THREAD)
  __declspec(thread) static GC* gc = nullptr;
#elif defined(HAS_ATTR_THREAD)
  static __thread GC* gc = nullptr;
#else
#error Need thread-local storage
#endif
  return gc;
}

bool GC::locked() {
  assert(gc());
  return gc()->_lockCount > 0;
}

GCLock::GCLock() { GC::lock(); }
GCLock::~GCLock() { GC::unlock(); }

class FreeListNode : public ASTNode {
public:
  FreeListNode* next;
  size_t size;
  FreeListNode(size_t s, FreeListNode* n) : ASTNode(ASTNode::NID_FL), next(n), size(s) {
    _gcMark = 1;
  }
  FreeListNode(size_t s) : ASTNode(ASTNode::NID_FL), next(nullptr), size(s) {}
};

class HeapPage {
public:
  HeapPage* next;
  size_t size;
  size_t used;
  char data[1];
  HeapPage(HeapPage* n, size_t s) : next(n), size(s), used(0) {}
};

/// Memory managed by the garbage collector
class GC::Heap {
  friend class GC;
#if defined(MINIZINC_GC_STATS)
  static const char* _nodeid[Item::II_END + 1];
  std::map<int, GCStat> gc_stats;
#endif
protected:
  static const size_t _min_gcThreshold;

  HeapPage* _page;
  GCMarker* _rootset;
  KeepAlive* _roots;
  WeakRef* _weakRefs;
  ASTNodeWeakMap* _nodeWeakMaps;
  static const int _max_fl = 5;
  FreeListNode* _fl[_max_fl + 1];
  static const size_t _fl_size[_max_fl + 1];
  static int freelistSlot(size_t _size) {
    size_t size = _size;
    assert(size <= _fl_size[_max_fl]);
    assert(size >= _fl_size[0]);
    size -= sizeof(Item);
    assert(size % sizeof(void*) == 0);
    size /= sizeof(void*);
    assert(size >= 1);
    int slot = static_cast<int>(size) - 1;
    return slot;
  }

  /// Total amount of memory allocated
  size_t _allocedMem;
  /// Total amount of memory currently free
  size_t _freeMem;
  /// Memory threshold for next garbage collection
  size_t _gcThreshold;
  /// High water mark of all allocated memory
  size_t _maxAllocedMem;

  /// A trail item
  struct TItem {
    Expression** l;
    Expression* v;
    bool mark;
    TItem(Expression** l0, Expression* v0) : l(l0), v(v0), mark(false) {}
  };
  /// Trail
  std::vector<TItem> _trail;

  Heap()
      : _page(nullptr),
        _rootset(nullptr),
        _roots(nullptr),
        _weakRefs(nullptr),
        _nodeWeakMaps(nullptr),
        _allocedMem(0),
        _freeMem(0),
        _gcThreshold(_min_gcThreshold),
        _maxAllocedMem(0) {
    for (int i = _max_fl + 1; (i--) != 0;) {
      _fl[i] = nullptr;
    }
  }

  /// Default size of pages to allocate
  static const size_t pageSize = 1 << 20;

  HeapPage* allocPage(size_t s, bool exact = false) {
    if (!exact) {
      s = std::max(s, pageSize);
    }
    auto* newPage = static_cast<HeapPage*>(::malloc(sizeof(HeapPage) + s - 1));
    if (newPage == nullptr) {
      throw InternalError("out of memory");
    }
#ifndef NDEBUG
    memset(newPage, 255, sizeof(HeapPage) + s - 1);
#endif
    _allocedMem += s;
    _maxAllocedMem = std::max(_maxAllocedMem, _allocedMem);
    _freeMem += s;
    if (exact && (_page != nullptr)) {
      new (newPage) HeapPage(_page->next, s);
      _page->next = newPage;
    } else {
      if (_page != nullptr) {
        size_t ns = _page->size - _page->used;
        assert(ns <= _fl_size[_max_fl]);
        if (ns >= _fl_size[0]) {
          // Remainder of page can be added to free lists
          auto* fln = reinterpret_cast<FreeListNode*>(_page->data + _page->used);
          _page->used += ns;
          new (fln) FreeListNode(ns, _fl[freelistSlot(ns)]);
          _fl[freelistSlot(ns)] = fln;
        } else {
          // Waste a little memory (less than smallest free list slot)
          _freeMem -= ns;
          assert(_allocedMem >= _freeMem);
        }
      }
      new (newPage) HeapPage(_page, s);
      _page = newPage;
    }
    return newPage;
  }

  void* alloc(size_t size, bool exact = false) {
    assert(size <= 80 || exact);
    /// Align to word boundary
    size += ((8 - (size & 7)) & 7);
    HeapPage* p = _page;
    if (exact || _page == nullptr || _page->used + size >= _page->size) {
      p = allocPage(size, exact);
    }
    char* ret = p->data + p->used;
    p->used += size;
    _freeMem -= size;
    if (p->size - p->used < _fl_size[0]) {
      _freeMem -= (p->size - p->used);
      _allocedMem -= (p->size - p->used);
      p->size = p->used;
    }
    assert(_allocedMem >= _freeMem);
    return ret;
  }

  /// Allocate one object of type T (no initialisation)
  template <typename T>
  T* alloc() {
    return static_cast<T*>(alloc(sizeof(T)));
  }

  /// Allocate \a n objects of type T (no initialisation)
  template <typename T>
  T* alloc(int n) {
    return static_cast<T*>(alloc(n * sizeof(T)));
  }

  void* fl(size_t size) {
    int slot = freelistSlot(size);
    assert(slot <= _max_fl);
    if (_fl[slot] != nullptr) {
      FreeListNode* p = _fl[slot];
      _fl[slot] = p->next;
      _freeMem -= size;
      return p;
    }
    return alloc(size);
  }

  void trigger() {
#ifdef MINIZINC_GC_STATS
    std::cerr << "GC\n\talloced " << (_allocedMem / 1024) << "\n\tfree " << (_freeMem / 1024)
              << "\n\tdiff " << ((_allocedMem - _freeMem) / 1024) << "\n\tthreshold "
              << (_gcThreshold / 1024) << "\n";
#endif
    size_t old_free = _freeMem;
    mark();
    sweep();
    // GC strategy:
    // increase threshold if either
    //   a) we haven't been able to put much on the free list (comapred to before GC), or
    //   b) the free list memory (after GC) is less than 50% of the allocated memory
    // otherwise (i.e., we have been able to increase the free list, and it is now more
    // than 50% of overall allocated memory), keep threshold at allocated memory
    if ((old_free != 0 && static_cast<double>(old_free) / static_cast<double>(_freeMem) > 0.9) ||
        static_cast<double>(_freeMem) / _allocedMem < 0.5) {
      _gcThreshold = std::max(_min_gcThreshold, static_cast<size_t>(_allocedMem * 1.5));
    } else {
      _gcThreshold = std::max(_min_gcThreshold, _allocedMem);
    }
#ifdef MINIZINC_GC_STATS
    std::cerr << "done\n\talloced " << (_allocedMem / 1024) << "\n\tfree " << (_freeMem / 1024)
              << "\n\tdiff " << ((_allocedMem - _freeMem) / 1024) << "\n\tthreshold "
              << (_gcThreshold / 1024) << "\n";
#endif
  }
  void rungc() {
    if (_allocedMem > _gcThreshold) {
      trigger();
    }
  }
  void mark();
  void sweep();

  static size_t nodesize(ASTNode* n) {
    static const size_t _nodesize[Item::II_END + 1] = {
        0,                      // NID_FL
        0,                      // NID_CHUNK
        0,                      // NID_VEC
        0,                      // NID_STR
        sizeof(IntLit),         // E_INTLIT
        sizeof(FloatLit),       // E_FLOATLIT
        sizeof(SetLit),         // E_SETLIT
        sizeof(BoolLit),        // E_BOOLLIT
        sizeof(StringLit),      // E_STRINGLIT
        sizeof(Id),             // E_ID
        sizeof(AnonVar),        // E_ANON
        sizeof(ArrayLit),       // E_ARRAYLIT
        sizeof(ArrayAccess),    // E_ARRAYACCESS
        sizeof(Comprehension),  // E_COMP
        sizeof(ITE),            // E_ITE
        sizeof(BinOp),          // E_BINOP
        sizeof(UnOp),           // E_UNOP
        sizeof(Call),           // E_CALL
        sizeof(VarDecl),        // E_VARDECL
        sizeof(Let),            // E_LET
        sizeof(TypeInst),       // E_TI
        sizeof(TIId),           // E_TIID
        sizeof(IncludeI),       // II_INC
        sizeof(VarDeclI),       // II_VD
        sizeof(AssignI),        // II_ASN
        sizeof(ConstraintI),    // II_CON
        sizeof(SolveI),         // II_SOL
        sizeof(OutputI),        // II_OUT
        sizeof(FunctionI)       // II_FUN
    };
    size_t ns;
    switch (static_cast<int>(n->_id)) {
      case ASTNode::NID_FL:
        ns = static_cast<FreeListNode*>(n)->size;
        break;
      case ASTNode::NID_CHUNK:
      case ASTNode::NID_STR:
        ns = static_cast<ASTChunk*>(n)->memsize();
        break;
      case ASTNode::NID_VEC:
        ns = static_cast<ASTVec*>(n)->memsize();
        break;
      case Call::eid:
        ns = n->_flag1 ? _nodesize[BinOp::eid] : _nodesize[Call::eid];
        break;
      default:
        assert(n->_id <= Item::II_END);
        ns = _nodesize[n->_id];
        break;
    }
    ns += ((8 - (ns & 7)) & 7);
    return ns;
  }
};

const size_t GC::Heap::_min_gcThreshold = 10LL * 1024LL;

#ifdef MINIZINC_GC_STATS
const char* GC::Heap::_nodeid[] = {
    "FreeList      ",  // NID_FL
    "Chunk         ",  // NID_CHUNK
    "Vec           ",  // NID_VEC
    "Str           ",  // NID_STR
    "IntLit        ",  // E_INTLIT
    "FloatLit      ",  // E_FLOATLIT
    "SetLit        ",  // E_SETLIT
    "BoolLit       ",  // E_BOOLLIT
    "StringLit     ",  // E_STRINGLIT
    "Id            ",  // E_ID
    "AnonVar       ",  // E_ANON
    "ArrayLit      ",  // E_ARRAYLIT
    "ArrayAccess   ",  // E_ARRAYACCESS
    "Comprehension ",  // E_COMP
    "ITE           ",  // E_ITE
    "BinOp         ",  // E_BINOP
    "UnOp          ",  // E_UNOP
    "Call          ",  // E_CALL
    "VarDecl       ",  // E_VARDECL
    "Let           ",  // E_LET
    "TypeInst      ",  // E_TI
    "TIId          ",  // E_TIID
    "IncludeI      ",  // II_INC
    "VarDeclI      ",  // II_VD
    "AssignI       ",  // II_ASN
    "ConstraintI   ",  // II_CON
    "SolveI        ",  // II_SOL
    "OutputI       ",  // II_OUT
    "FunctionI     "   // II_FUN
};
#endif

void GC::setTimeout(unsigned long long int t) {
  if (gc() == nullptr) {
    gc() = new GC();
  }
  gc()->_timeout = t;
  gc()->_timeoutTimer.reset();
}

void GC::lock() {
  if (gc() == nullptr) {
    gc() = new GC();
  }
  // If a timeout has been specified, first check counter
  // before checking timer (counter is much cheaper, introduces
  // less overhead)
  if (gc()->_timeout > 0 && gc()->_timeoutCount++ > 500) {
    gc()->_timeoutCount = 0;
    if (gc()->_timeoutTimer.ms() > gc()->_timeout) {
      gc()->_timeout = 0;
      gc()->_timeoutCount = 0;
      throw Timeout();
    }
  }
  if (gc()->_lockCount == 0) {
    gc()->_heap->rungc();
  }
  gc()->_lockCount++;
}
void GC::unlock() {
  assert(locked());
  gc()->_lockCount--;
}

void GC::trigger() {
  if (!locked()) {
    gc()->_heap->trigger();
  }
}

const size_t GC::Heap::pageSize;

const size_t GC::Heap::_fl_size[GC::Heap::_max_fl + 1] = {
    sizeof(Item) + 1 * sizeof(void*), sizeof(Item) + 2 * sizeof(void*),
    sizeof(Item) + 3 * sizeof(void*), sizeof(Item) + 4 * sizeof(void*),
    sizeof(Item) + 5 * sizeof(void*), sizeof(Item) + 6 * sizeof(void*),
};

GC::GC() : _heap(new Heap()), _lockCount(0), _timeout(0), _timeoutCount(0) {}

void GC::add(GCMarker* m) {
  GC* gc = GC::gc();
  if (gc->_heap->_rootset != nullptr) {
    m->_rootsNext = gc->_heap->_rootset;
    m->_rootsPrev = m->_rootsNext->_rootsPrev;
    m->_rootsPrev->_rootsNext = m;
    m->_rootsNext->_rootsPrev = m;
  } else {
    gc->_heap->_rootset = m->_rootsNext = m->_rootsPrev = m;
  }
}

void GC::remove(GCMarker* m) {
  GC* gc = GC::gc();
  if (m->_rootsNext == m) {
    gc->_heap->_rootset = nullptr;
  } else {
    m->_rootsNext->_rootsPrev = m->_rootsPrev;
    m->_rootsPrev->_rootsNext = m->_rootsNext;
    if (m == gc->_heap->_rootset) {
      gc->_heap->_rootset = m->_rootsPrev;
    }
  }
}

void* GC::alloc(size_t size) {
  assert(locked());
  void* ret;
  if (size < GC::Heap::_fl_size[0] || size > GC::Heap::_fl_size[GC::Heap::_max_fl]) {
    ret = _heap->alloc(size, true);
  } else {
    ret = _heap->fl(size);
  }
  new (ret) FreeListNode(size);
  return ret;
}

void GC::Heap::mark() {
#if defined(MINIZINC_GC_STATS)
  std::cerr << "================= mark =================: ";
  gc_stats.clear();
#endif

  for (KeepAlive* e = _roots; e != nullptr; e = e->next()) {
    if (((*e)() != nullptr) && (*e)()->_gcMark == 0U) {
      Expression::mark((*e)());
#if defined(MINIZINC_GC_STATS)
      gc_stats[(*e)()->_id].keepalive++;
#endif
    }
  }
#if defined(MINIZINC_GC_STATS)
  std::cerr << "+";
#endif

  GCMarker* m = _rootset;
  if (m == nullptr) {
    return;
  }
  do {
    m->mark();
    m = m->_rootsNext;
  } while (m != _rootset);

  for (auto i = static_cast<unsigned int>(_trail.size()); (i--) != 0U;) {
    Expression::mark(_trail[i].v);
  }

  bool fixPrev = false;
  WeakRef* prevWr = nullptr;
  for (WeakRef* wr = _weakRefs; wr != nullptr; wr = wr->next()) {
    if (fixPrev) {
      fixPrev = false;
      removeWeakRef(prevWr);
      prevWr->_n = nullptr;
      prevWr->_p = nullptr;
    }
    if (((*wr)() != nullptr) && (*wr)()->_gcMark == 0U) {
      wr->_e = nullptr;
      wr->_valid = false;
      fixPrev = true;
      prevWr = wr;
    }
  }
  if (fixPrev) {
    removeWeakRef(prevWr);
    prevWr->_n = nullptr;
    prevWr->_p = nullptr;
  }

  for (ASTNodeWeakMap* wr = _nodeWeakMaps; wr != nullptr; wr = wr->next()) {
    std::vector<ASTNode*> toRemove;
    for (auto n : wr->_m) {
      if (n.first->_gcMark == 0U || n.second->_gcMark == 0U) {
        toRemove.push_back(n.first);
      }
    }
    for (auto* n : toRemove) {
      wr->_m.erase(n);
    }
  }

#if defined(MINIZINC_GC_STATS)
  std::cerr << "+";
  std::cerr << "\n";
#endif
}

void GC::Heap::sweep() {
#if defined(MINIZINC_GC_STATS)
  std::cerr << "=============== GC sweep =============\n";
#endif
  HeapPage* p = _page;
  HeapPage* prev = nullptr;
  while (p != nullptr) {
    size_t off = 0;
    bool wholepage = true;
    struct NodeInfo {
      ASTNode* n;
      size_t ns;
      NodeInfo(ASTNode* n0, size_t ns0) : n(n0), ns(ns0) {}
    };
    std::vector<NodeInfo> freeNodes;
    freeNodes.reserve(100);
    while (off < p->used) {
      auto* n = reinterpret_cast<ASTNode*>(p->data + off);
      size_t ns = nodesize(n);
      assert(ns != 0);
#if defined(MINIZINC_GC_STATS)
      GCStat& stats = gc_stats[n->_id];
      stats.first++;
      stats.total += ns;
#endif
      if (n->_gcMark == 0U) {
        switch (static_cast<int>(n->_id)) {
          case Item::II_FUN:
            static_cast<FunctionI*>(n)->ann().~Annotation();
            break;
          case Item::II_SOL:
            static_cast<SolveI*>(n)->ann().~Annotation();
            break;
          case ASTNode::NID_STR:
            static_cast<ASTStringData*>(n)->destroy();
            break;
          case Expression::E_VARDECL:
            // Reset WeakRef inside VarDecl
            static_cast<VarDecl*>(n)->flat(nullptr);
            // fall through
          default:
            if (n->_id >= static_cast<unsigned int>(ASTNode::NID_END) + 1 &&
                n->_id <= static_cast<unsigned int>(Expression::EID_END)) {
              static_cast<Expression*>(n)->ann().~Annotation();
            }
        }
        if (ns >= _fl_size[0] && ns <= _fl_size[_max_fl]) {
          freeNodes.emplace_back(n, ns);
        } else {
          assert(off == 0);
          assert(p->used == p->size);
        }
      } else {
#if defined(MINIZINC_GC_STATS)
        stats.second++;
#endif
        wholepage = false;
        if (n->_id != static_cast<unsigned int>(ASTNode::NID_FL)) {
          n->_gcMark = 0;
        }
      }
      off += ns;
    }
    if (wholepage) {
#ifndef NDEBUG
      memset(p->data, 42, p->size);
#endif
      if (prev != nullptr) {
        prev->next = p->next;
      } else {
        assert(p == _page);
        _page = p->next;
      }
      HeapPage* pf = p;
      p = p->next;
      _allocedMem -= pf->size;
      if (pf->size - pf->used >= _fl_size[0]) {
        _freeMem -= (pf->size - pf->used);
      }
      assert(_allocedMem >= _freeMem);
      ::free(pf);
    } else {
      for (auto ni : freeNodes) {
        auto* fln = static_cast<FreeListNode*>(ni.n);
        new (fln) FreeListNode(ni.ns, _fl[freelistSlot(ni.ns)]);
        _fl[freelistSlot(ni.ns)] = fln;
        _freeMem += ni.ns;
#if defined(MINIZINC_GC_STATS)
        gc_stats[fln->_id].second++;
#endif
      }
      assert(_allocedMem >= _freeMem);
      prev = p;
      p = p->next;
    }
  }
#if defined(MINIZINC_GC_STATS)
  for (auto stat : gc_stats) {
    std::cerr << _nodeid[stat.first] << ":\t" << stat.second.first << " / " << stat.second.second
              << " / " << stat.second.keepalive << " / " << stat.second.inmodel << " / ";
    if (stat.second.total > 1024) {
      if (stat.second.total > 1024 * 1024) {
        std::cerr << (stat.second.total / 1024 / 1024) << "M";
      } else {
        std::cerr << (stat.second.total / 1024) << "K";
      }
    } else {
      std::cerr << (stat.second.total);
    }
    std::cerr << std::endl;
  }
#endif
}

ASTVec::ASTVec(size_t size) : ASTNode(NID_VEC), _size(size) {}
void* ASTVec::alloc(size_t size) {
  size_t s = sizeof(ASTVec) + (size <= 2 ? 0 : size - 2) * sizeof(void*);
  s += ((8 - (s & 7)) & 7);
  return GC::gc()->alloc(s);
}

ASTChunk::ASTChunk(size_t size, unsigned int id) : ASTNode(id), _size(size) {}
void* ASTChunk::alloc(size_t size) {
  size_t s = sizeof(ASTChunk) + (size <= 4 ? 0 : size - 4) * sizeof(char);
  s += ((8 - (s & 7)) & 7);
  return GC::gc()->alloc(s);
}

void GC::mark() {
  GC* gc = GC::gc();
  if (!gc->_heap->_trail.empty()) {
    gc->_heap->_trail.back().mark = true;
  }
}
void GC::trail(Expression** l, Expression* v) {
  GC* gc = GC::gc();
  gc->_heap->_trail.emplace_back(l, v);
}
void GC::untrail() {
  GC* gc = GC::gc();
  while (!gc->_heap->_trail.empty() && !gc->_heap->_trail.back().mark) {
    *gc->_heap->_trail.back().l = gc->_heap->_trail.back().v;
    gc->_heap->_trail.pop_back();
  }
  if (!gc->_heap->_trail.empty()) {
    gc->_heap->_trail.back().mark = false;
  }
}
size_t GC::maxMem() {
  GC* gc = GC::gc();
  return gc->_heap->_maxAllocedMem;
}

void* ASTNode::operator new(size_t size) { return GC::gc()->alloc(size); }

void GC::addKeepAlive(KeepAlive* e) {
  assert(e->_p == nullptr);
  assert(e->_n == nullptr);
  e->_n = GC::gc()->_heap->_roots;
  if (GC::gc()->_heap->_roots != nullptr) {
    GC::gc()->_heap->_roots->_p = e;
  }
  GC::gc()->_heap->_roots = e;
}
void GC::removeKeepAlive(KeepAlive* e) {
  if (e->_p != nullptr) {
    e->_p->_n = e->_n;
  } else {
    assert(GC::gc()->_heap->_roots == e);
    GC::gc()->_heap->_roots = e->_n;
  }
  if (e->_n != nullptr) {
    e->_n->_p = e->_p;
  }
}

KeepAlive::KeepAlive(Expression* e) : _e(e), _p(nullptr), _n(nullptr) {
  if ((_e != nullptr) && !_e->isUnboxedVal()) {
    GC::gc()->addKeepAlive(this);
  }
}
KeepAlive::~KeepAlive() {
  if ((_e != nullptr) && !_e->isUnboxedVal()) {
    GC::gc()->removeKeepAlive(this);
  }
}
KeepAlive::KeepAlive(const KeepAlive& e) : _e(e._e), _p(nullptr), _n(nullptr) {
  if ((_e != nullptr) && !_e->isUnboxedVal()) {
    GC::gc()->addKeepAlive(this);
  }
}
KeepAlive& KeepAlive::operator=(const KeepAlive& e) {
  if (this != &e) {
    if ((_e != nullptr) && !_e->isUnboxedVal()) {
      if (e._e == nullptr || e._e->isUnboxedVal()) {
        GC::gc()->removeKeepAlive(this);
        _p = _n = nullptr;
      }
    } else {
      if (e._e != nullptr && !e._e->isUnboxedVal()) {
        GC::gc()->addKeepAlive(this);
      }
    }
    _e = e._e;
  }
  return *this;
}

void GC::addWeakRef(WeakRef* e) {
  assert(e->_p == nullptr);
  assert(e->_n == nullptr);
  e->_n = GC::gc()->_heap->_weakRefs;
  if (GC::gc()->_heap->_weakRefs != nullptr) {
    GC::gc()->_heap->_weakRefs->_p = e;
  }
  GC::gc()->_heap->_weakRefs = e;
}
void GC::removeWeakRef(MiniZinc::WeakRef* e) {
  if (e->_p != nullptr) {
    e->_p->_n = e->_n;
  } else {
    assert(GC::gc()->_heap->_weakRefs == e);
    GC::gc()->_heap->_weakRefs = e->_n;
  }
  if (e->_n != nullptr) {
    e->_n->_p = e->_p;
  }
}
void GC::addNodeWeakMap(ASTNodeWeakMap* m) {
  assert(m->_p == nullptr);
  assert(m->_n == nullptr);
  m->_n = GC::gc()->_heap->_nodeWeakMaps;
  if (GC::gc()->_heap->_nodeWeakMaps != nullptr) {
    GC::gc()->_heap->_nodeWeakMaps->_p = m;
  }
  GC::gc()->_heap->_nodeWeakMaps = m;
}
void GC::removeNodeWeakMap(ASTNodeWeakMap* m) {
  if (m->_p != nullptr) {
    m->_p->_n = m->_n;
  } else {
    assert(GC::gc()->_heap->_nodeWeakMaps == m);
    GC::gc()->_heap->_nodeWeakMaps = m->_n;
  }
  if (m->_n != nullptr) {
    m->_n->_p = m->_p;
  }
}

WeakRef::WeakRef(Expression* e) : _e(e), _p(nullptr), _n(nullptr), _valid(true) {
  if ((_e != nullptr) && !_e->isUnboxedVal()) {
    GC::gc()->addWeakRef(this);
  }
}
WeakRef::~WeakRef() {
  if ((_e != nullptr) && !_e->isUnboxedVal()) {
    GC::gc()->removeWeakRef(this);
  }
}
WeakRef::WeakRef(const WeakRef& e) : _e(e()), _p(nullptr), _n(nullptr), _valid(true) {
  if ((_e != nullptr) && !_e->isUnboxedVal()) {
    GC::gc()->addWeakRef(this);
  }
}
WeakRef& WeakRef::operator=(const WeakRef& e) {
  if (this != &e) {
    // Test if this WeakRef is currently active in the GC
    bool isActive = ((_e != nullptr) && !_e->isUnboxedVal());
    if (isActive) {
      // Yes, active WeakRef.
      // If after assigning WeakRef should be inactive, remove it.
      if (e() == nullptr || e()->isUnboxedVal()) {
        GC::gc()->removeWeakRef(this);
        _n = _p = nullptr;
      }
    }
    _e = e();
    _valid = true;

    // If this WeakRef was not active but now should be, add it
    if (!isActive && _e != nullptr && !_e->isUnboxedVal()) {
      GC::gc()->addWeakRef(this);
    }
  }
  return *this;
}

ASTNodeWeakMap::ASTNodeWeakMap() : _p(nullptr), _n(nullptr) { GC::gc()->addNodeWeakMap(this); }

ASTNodeWeakMap::~ASTNodeWeakMap() { GC::gc()->removeNodeWeakMap(this); }

void ASTNodeWeakMap::insert(ASTNode* n0, ASTNode* n1) { _m.insert(std::make_pair(n0, n1)); }

ASTNode* ASTNodeWeakMap::find(ASTNode* n) {
  auto it = _m.find(n);
  if (it == _m.end()) {
    return nullptr;
  }
  return it->second;
}

}  // namespace MiniZinc
