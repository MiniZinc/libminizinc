/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/gc.hh>
#include <minizinc/ast.hh>
#include <minizinc/hash.hh>
#include <minizinc/model.hh>
#include <minizinc/config.hh>

#include <vector>
#include <cstring>

//#define MINIZINC_GC_STATS

#if defined(MINIZINC_GC_STATS)
#include <map>
#endif

namespace MiniZinc {
  
  GC*&
  GC::gc(void) {
#if defined(HAS_DECLSPEC_THREAD)
    __declspec (thread) static GC* gc = NULL;
#elif defined(HAS_ATTR_THREAD)
    static __thread GC* gc = NULL;
#else
#error Need thread-local storage
#endif
    return gc;
  }
    
  bool
  GC::locked(void) {
    assert(gc());
    return gc()->_lock_count > 0;
  }

  GCLock::GCLock(void) {
    GC::lock();
  }
  GCLock::~GCLock(void) {
    GC::unlock();
  }

  class FreeListNode : public ASTNode {
  public:
    FreeListNode* next;
    size_t size;
    FreeListNode(size_t s, FreeListNode* n)
      : ASTNode(ASTNode::NID_FL)
      , next(n)
      , size(s) {
      _gc_mark = 1;
    }
    FreeListNode(size_t s) : ASTNode(ASTNode::NID_FL), next(NULL), size(s) {}
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
    static const char* _nodeid[Item::II_END+1];
    struct GCStat {
      int first;
      int second;
      int keepalive;
      int inmodel;
      size_t total;
      GCStat(void) : first(0), second(0), keepalive(0), inmodel(0), total(0) {}
    };
    std::map<int,GCStat> gc_stats;
#endif
  protected:
    HeapPage* _page;
    Model* _rootset;
    KeepAlive* _roots;
    WeakRef* _weakRefs;
    ASTNodeWeakMap* _nodeWeakMaps;
    static const int _max_fl = 5;
    FreeListNode* _fl[_max_fl+1];
    static const size_t _fl_size[_max_fl+1];
    int _fl_slot(size_t _size) {
      size_t size = _size;
      assert(size <= _fl_size[_max_fl]);
      assert(size >= _fl_size[0]);
      size -= sizeof(Item);
      assert(size % sizeof(void*) == 0);
      size /= sizeof(void*);
      assert(size >= 1);
      int slot = static_cast<int>(size)-1;
      return slot;
    }

    /// Total amount of memory allocated
    size_t _alloced_mem;
    /// Total amount of memory currently free
    size_t _free_mem;
    /// Memory threshold for next garbage collection
    size_t _gc_threshold;
    /// High water mark of all allocated memory
    size_t _max_alloced_mem;

    /// A trail item
    struct TItem {
      Expression** l;
      Expression* v;
      bool mark;
      TItem(Expression** l0, Expression* v0)
        : l(l0), v(v0), mark(false) {}
    };
    /// Trail
    std::vector<TItem> trail;

    Heap(void)
      : _page(NULL)
      , _rootset(NULL)
      , _roots(NULL)
      , _weakRefs(NULL)
      , _nodeWeakMaps(NULL)
      , _alloced_mem(0)
      , _free_mem(0)
      , _gc_threshold(10)
      , _max_alloced_mem(0) {
      for (int i=_max_fl+1; i--;)
        _fl[i] = NULL;
    }

    /// Default size of pages to allocate
    static const size_t pageSize = 1<<20;
    
    HeapPage* allocPage(size_t s, bool exact=false) {
      if (!exact)
        s = std::max(s,pageSize);
      HeapPage* newPage =
        static_cast<HeapPage*>(::malloc(sizeof(HeapPage)+s-1));
      if (newPage==NULL) {
        throw InternalError("out of memory");
      }
#ifndef NDEBUG
      memset(newPage,255,sizeof(HeapPage)+s-1);
#endif
      _alloced_mem += s;
      _max_alloced_mem = std::max(_max_alloced_mem, _alloced_mem);
      _free_mem += s;
      if (exact && _page) {
        new (newPage) HeapPage(_page->next,s);
        _page->next = newPage;
      } else {
        if (_page) {
          size_t ns = _page->size-_page->used;
          assert(ns <= _fl_size[_max_fl]);
          if (ns >= _fl_size[0]) {
            // Remainder of page can be added to free lists
            FreeListNode* fln = 
              reinterpret_cast<FreeListNode*>(_page->data+_page->used);
            _page->used += ns;
            new (fln) FreeListNode(ns, _fl[_fl_slot(ns)]);
            _fl[_fl_slot(ns)] = fln;
          } else {
            // Waste a little memory (less than smallest free list slot)
            _free_mem -= ns;
            assert(_alloced_mem >= _free_mem);
          }
        }
        new (newPage) HeapPage(_page,s);
        _page = newPage;
      }
      return newPage;
    }

    void*
    alloc(size_t size, bool exact=false) {
      assert(size<=80 || exact);
      /// Align to word boundary
      size += ((8 - (size & 7)) & 7);
      HeapPage* p = _page;
      if (exact || _page==NULL || _page->used+size >= _page->size)
        p = allocPage(size,exact);
      char* ret = p->data+p->used;
      p->used += size;
      _free_mem -= size;
      assert(_alloced_mem >= _free_mem);
      return ret;
    }

    /// Allocate one object of type T (no initialisation)
    template<typename T>
    T* alloc(void) { return static_cast<T*>(alloc(sizeof(T))); }
  
    /// Allocate \a n objects of type T (no initialisation)
    template<typename T>
    T* alloc(int n) { return static_cast<T*>(alloc(n*sizeof(T))); }

    void* fl(size_t size) {
      int slot = _fl_slot(size);
      assert(slot <= _max_fl);
      if (_fl[slot]) {
        FreeListNode* p = _fl[slot];
        _fl[slot] = p->next;
        _free_mem -= size;
        return p;
      }
      return alloc(size);
    }

    void trigger(void) {
#ifdef MINIZINC_GC_STATS
      std::cerr << "GC\n\talloced " << (_alloced_mem/1024) << "\n\tfree " << (_free_mem/1024) << "\n\tdiff "
      << ((_alloced_mem-_free_mem)/1024)
      << "\n\tthreshold " << (_gc_threshold/1024)
      << "\n";
#endif
      mark();
      sweep();
      _gc_threshold = static_cast<size_t>(_alloced_mem * 1.5);
#ifdef MINIZINC_GC_STATS
      std::cerr << "done\n\talloced " << (_alloced_mem/1024) << "\n\tfree " << (_free_mem/1024) << "\n\tdiff "
      << ((_alloced_mem-_free_mem)/1024)
      << "\n\tthreshold " << (_gc_threshold/1024)
      << "\n";
#endif
    }
    void rungc(void) {
      if (_alloced_mem > _gc_threshold) {
        trigger();
      }
    }
    void mark(void);
    void sweep(void);

    static size_t
    nodesize(ASTNode* n) {
      static const size_t _nodesize[Item::II_END+1] = {
        0, // NID_FL
        0, // NID_CHUNK
        0, // NID_VEC
        sizeof(IntLit),        // E_INTLIT
        sizeof(FloatLit),      // E_FLOATLIT
        sizeof(SetLit),        // E_SETLIT
        sizeof(BoolLit),       // E_BOOLLIT
        sizeof(StringLit),     // E_STRINGLIT
        sizeof(Id),            // E_ID
        sizeof(AnonVar),       // E_ANON
        sizeof(ArrayLit),      // E_ARRAYLIT
        sizeof(ArrayAccess),   // E_ARRAYACCESS
        sizeof(Comprehension), // E_COMP
        sizeof(ITE),           // E_ITE
        sizeof(BinOp),         // E_BINOP
        sizeof(UnOp),          // E_UNOP
        sizeof(Call),          // E_CALL
        sizeof(VarDecl),       // E_VARDECL
        sizeof(Let),           // E_LET
        sizeof(TypeInst),      // E_TI
        sizeof(TIId),          // E_TIID
        sizeof(IncludeI),      // II_INC
        sizeof(VarDeclI),      // II_VD
        sizeof(AssignI),       // II_ASN
        sizeof(ConstraintI),   // II_CON
        sizeof(SolveI),        // II_SOL
        sizeof(OutputI),       // II_OUT
        sizeof(FunctionI)      // II_FUN
      };
      size_t ns;
      switch (n->_id) {
      case ASTNode::NID_FL:
        ns = static_cast<FreeListNode*>(n)->size;
        break;
      case ASTNode::NID_CHUNK:
        ns = static_cast<ASTChunk*>(n)->memsize();
        break;
      case ASTNode::NID_VEC:
        ns = static_cast<ASTVec*>(n)->memsize();
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

#ifdef MINIZINC_GC_STATS
  const char*
  GC::Heap::_nodeid[] = {
    "FreeList      ", // NID_FL
    "Chunk         ", // NID_CHUNK
    "Vec           ", // NID_VEC
    "IntLit        ",        // E_INTLIT
    "FloatLit      ",      // E_FLOATLIT
    "SetLit        ",        // E_SETLIT
    "BoolLit       ",       // E_BOOLLIT
    "StringLit     ",     // E_STRINGLIT
    "Id            ",            // E_ID
    "AnonVar       ",       // E_ANON
    "ArrayLit      ",      // E_ARRAYLIT
    "ArrayAccess   ",   // E_ARRAYACCESS
    "Comprehension ", // E_COMP
    "ITE           ",           // E_ITE
    "BinOp         ",         // E_BINOP
    "UnOp          ",          // E_UNOP
    "Call          ",          // E_CALL
    "VarDecl       ",       // E_VARDECL
    "Let           ",           // E_LET
    "TypeInst      ",      // E_TI
    "TIId          ",          // E_TIID
    "IncludeI      ",      // II_INC
    "VarDeclI      ",      // II_VD
    "AssignI       ",       // II_ASN
    "ConstraintI   ",   // II_CON
    "SolveI        ",        // II_SOL
    "OutputI       ",       // II_OUT
    "FunctionI     "      // II_FUN
  };
#endif
  
  void
  GC::lock(void) {
    if (gc()==NULL) {
      gc() = new GC();
    }
    if (gc()->_lock_count==0)
      gc()->_heap->rungc();
    gc()->_lock_count++;
  }
  void
  GC::unlock(void) {
    assert(locked());
    gc()->_lock_count--;
  }

  void GC::trigger(void) {
    if (!locked())
      gc()->_heap->trigger();
  }
  
  const size_t GC::Heap::pageSize;

  const size_t
  GC::Heap::_fl_size[GC::Heap::_max_fl+1] = {
    sizeof(Item)+1*sizeof(void*),
    sizeof(Item)+2*sizeof(void*),
    sizeof(Item)+3*sizeof(void*),
    sizeof(Item)+4*sizeof(void*),
    sizeof(Item)+5*sizeof(void*),
    sizeof(Item)+6*sizeof(void*),
  };

  GC::GC(void) : _heap(new Heap()), _lock_count(0) {}

  void
  GC::add(Model* m) {
    GC* gc = GC::gc();
    if (gc->_heap->_rootset) {
      m->_roots_next = gc->_heap->_rootset;
      m->_roots_prev = m->_roots_next->_roots_prev;
      m->_roots_prev->_roots_next = m;
      m->_roots_next->_roots_prev = m;
    } else {
      gc->_heap->_rootset = m->_roots_next = m->_roots_prev = m;
    }
  }

  void
  GC::remove(Model* m) {
    GC* gc = GC::gc();
    if (m->_roots_next == m) {
      gc->_heap->_rootset = NULL;
    } else {
      m->_roots_next->_roots_prev = m->_roots_prev;
      m->_roots_prev->_roots_next = m->_roots_next;
      if (m==gc->_heap->_rootset)
        gc->_heap->_rootset = m->_roots_prev;
    }
  }

  void*
  GC::alloc(size_t size) {
    assert(locked());
    void* ret;
    if (size < _heap->_fl_size[0] || size > _heap->_fl_size[_heap->_max_fl]) {
      ret = _heap->alloc(size,true);
    } else {
      ret = _heap->fl(size);
    }
    new (ret) FreeListNode(size);
    return ret;
  }

  void
  GC::Heap::mark(void) {
#if defined(MINIZINC_GC_STATS)
    std::cerr << "================= mark =================: ";
    gc_stats.clear();
#endif

    for (KeepAlive* e = _roots; e != NULL; e = e->next()) {
      if ((*e)() && (*e)()->_gc_mark==0) {
        Expression::mark((*e)());
#if defined(MINIZINC_GC_STATS)
        gc_stats[(*e)()->_id].keepalive++;
#endif
      }
    }
#if defined(MINIZINC_GC_STATS)
    std::cerr << "+";
#endif
    
    Model* m = _rootset;
    if (m==NULL)
      return;
    do {
      m->_filepath.mark();
      m->_filename.mark();
      for (unsigned int j=0; j<m->_items.size(); j++) {
        Item* i = m->_items[j];
        if (i->_gc_mark==0) {
          i->_gc_mark = 1;
          i->loc().mark();
          switch (i->iid()) {
          case Item::II_INC:
            i->cast<IncludeI>()->f().mark();
            break;
          case Item::II_VD:
            Expression::mark(i->cast<VarDeclI>()->e());
#if defined(MINIZINC_GC_STATS)
            gc_stats[i->cast<VarDeclI>()->e()->Expression::eid()].inmodel++;
#endif
            break;
          case Item::II_ASN:
            i->cast<AssignI>()->id().mark();
            Expression::mark(i->cast<AssignI>()->e());
            Expression::mark(i->cast<AssignI>()->decl());
            break;
          case Item::II_CON:
            Expression::mark(i->cast<ConstraintI>()->e());
#if defined(MINIZINC_GC_STATS)
            gc_stats[i->cast<ConstraintI>()->e()->Expression::eid()].inmodel++;
#endif
            break;
          case Item::II_SOL:
            {
              SolveI* si = i->cast<SolveI>();
              for (ExpressionSetIter it = si->ann().begin(); it != si->ann().end(); ++it) {
                Expression::mark(*it);
              }
            }
            Expression::mark(i->cast<SolveI>()->e());
            break;
          case Item::II_OUT:
            Expression::mark(i->cast<OutputI>()->e());
            break;
          case Item::II_FUN:
            {
              FunctionI* fi = i->cast<FunctionI>();
              fi->id().mark();
              Expression::mark(fi->ti());
              for (ExpressionSetIter it = fi->ann().begin(); it != fi->ann().end(); ++it) {
                Expression::mark(*it);
              }
              Expression::mark(fi->e());
              fi->params().mark();
              for (unsigned int k=0; k<fi->params().size(); k++) {
                Expression::mark(fi->params()[k]);
              }
            }
            break;      
          }
        }
      }
      m = m->_roots_next;
    } while (m != _rootset);
    
    for (unsigned int i=trail.size(); i--;) {
      Expression::mark(trail[i].v);
    }
    
    bool fixPrev = false;
    WeakRef* prevWr = NULL;
    for (WeakRef* wr = _weakRefs; wr != NULL; wr = wr->next()) {
      if (fixPrev) {
        fixPrev = false;
        removeWeakRef(prevWr);
        prevWr->_n = NULL;
        prevWr->_p = NULL;
      }
      if ((*wr)() && (*wr)()->_gc_mark==0) {
        wr->_e = NULL;
        wr->_valid = false;
        fixPrev = true;
        prevWr = wr;
      }
    }
    if (fixPrev) {
      removeWeakRef(prevWr);
      prevWr->_n = NULL;
      prevWr->_p = NULL;
    }
    
    for (ASTNodeWeakMap* wr = _nodeWeakMaps; wr != NULL; wr = wr->next()) {
      std::vector<ASTNode*> toRemove;
      for (auto n : wr->_m) {
        if (n.first->_gc_mark==0 || n.second->_gc_mark==0)
          toRemove.push_back(n.first);
      }
      for (auto n : toRemove) {
        wr->_m.erase(n);
      }
    }

#if defined(MINIZINC_GC_STATS)
    std::cerr << "+";
    std::cerr << "\n";
#endif
  }
    
  void
  GC::Heap::sweep(void) {
#if defined(MINIZINC_GC_STATS)
    std::cerr << "=============== GC sweep =============\n";
#endif
    HeapPage* p = _page;
    HeapPage* prev = NULL;
    while (p) {
      size_t off = 0;
      bool wholepage = false;
      while (off < p->used) {
        ASTNode* n = reinterpret_cast<ASTNode*>(p->data+off);
        size_t ns = nodesize(n);
        assert(ns != 0);
#if defined(MINIZINC_GC_STATS)
        GCStat& stats = gc_stats[n->_id];
        stats.first++;
        stats.total += ns;
#endif
        if (n->_gc_mark==0) {
          switch (n->_id) {
            case Item::II_FUN:
              static_cast<FunctionI*>(n)->ann().~Annotation();
              break;
            case Item::II_SOL:
              static_cast<SolveI*>(n)->ann().~Annotation();
              break;
            case Expression::E_VARDECL:
              // Reset WeakRef inside VarDecl
              static_cast<VarDecl*>(n)->flat(NULL);
              // fall through
            default:
              if (n->_id >= ASTNode::NID_END+1 && n->_id <= Expression::EID_END) {
                static_cast<Expression*>(n)->ann().~Annotation();
              }
          }
          if (ns >= _fl_size[0] && ns <= _fl_size[_max_fl]) {
            FreeListNode* fln = static_cast<FreeListNode*>(n);
            new (fln) FreeListNode(ns, _fl[_fl_slot(ns)]);
            _fl[_fl_slot(ns)] = fln;
            _free_mem += ns;
#if defined(MINIZINC_GC_STATS)
            gc_stats[fln->_id].second++;
#endif
            assert(_alloced_mem >= _free_mem);
          } else {
            assert(off==0);
            assert(p->used==p->size);
            wholepage = true;
          }
        } else {
#if defined(MINIZINC_GC_STATS)
          stats.second++;
#endif
          if (n->_id != ASTNode::NID_FL)
            n->_gc_mark=0;
        }
        off += ns;
      }
      if (wholepage) {
#ifndef NDEBUG
        memset(p->data,42,p->size);
#endif
        if (prev) {
          prev->next = p->next;
        } else {
          assert(p==_page);
          _page = p->next;
        }
        HeapPage* pf = p;
        p = p->next;
        _alloced_mem -= pf->size;
        assert(_alloced_mem >= _free_mem);
        ::free(pf);
      } else {
        prev = p;
        p = p->next;
      }
    }
#if defined(MINIZINC_GC_STATS)
    for (auto stat: gc_stats) {
      std::cerr << _nodeid[stat.first] << ":\t" << stat.second.first << " / " << stat.second.second
      << " / " << stat.second.keepalive << " / " << stat.second.inmodel << " / ";
      if (stat.second.total > 1024) {
        if (stat.second.total > 1024*1024) {
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

  ASTVec::ASTVec(size_t size)
    : ASTNode(NID_VEC), _size(size) {}
  void*
  ASTVec::alloc(size_t size) {
    size_t s = sizeof(ASTVec)+(size<=2?0:size-2)*sizeof(void*);
    s += ((8 - (s & 7)) & 7);
    return GC::gc()->alloc(s);
  }

  ASTChunk::ASTChunk(size_t size)
    : ASTNode(NID_CHUNK), _size(size) {}
  void*
  ASTChunk::alloc(size_t size) {
    size_t s = sizeof(ASTChunk)+(size<=4?0:size-4)*sizeof(char);
    s += ((8 - (s & 7)) & 7);
    return GC::gc()->alloc(s);
  }

  void
  GC::mark(void) {
    GC* gc = GC::gc();
    if (!gc->_heap->trail.empty())
      gc->_heap->trail.back().mark = true;
  }
  void
  GC::trail(Expression** l,Expression* v) {
    GC* gc = GC::gc();
    gc->_heap->trail.push_back(GC::Heap::TItem(l,v));
  }
  void
  GC::untrail(void) {
    GC* gc = GC::gc();
    while (!gc->_heap->trail.empty() && !gc->_heap->trail.back().mark) {
      *gc->_heap->trail.back().l = gc->_heap->trail.back().v;
      gc->_heap->trail.pop_back();
    }
    if (!gc->_heap->trail.empty())
      gc->_heap->trail.back().mark = false;
  }
  size_t
  GC::maxMem(void) {
    GC* gc = GC::gc();
    return gc->_heap->_max_alloced_mem;
  }
  

  void*
  ASTNode::operator new(size_t size) {
    return GC::gc()->alloc(size);
  }

  void
  GC::addKeepAlive(KeepAlive* e) {
    assert(e->_p==NULL);
    assert(e->_n==NULL);
    e->_n = GC::gc()->_heap->_roots;
    if (GC::gc()->_heap->_roots)
      GC::gc()->_heap->_roots->_p = e;
    GC::gc()->_heap->_roots = e;
  }
  void
  GC::removeKeepAlive(KeepAlive* e) {
    if (e->_p) {
      e->_p->_n = e->_n;
    } else {
      assert(GC::gc()->_heap->_roots==e);
      GC::gc()->_heap->_roots = e->_n;
    }
    if (e->_n) {
      e->_n->_p = e->_p;
    }
  }

  KeepAlive::KeepAlive(Expression* e)
    : _e(e), _p(NULL), _n(NULL) {
    if (_e && !_e->isUnboxedVal())
      GC::gc()->addKeepAlive(this);
  }
  KeepAlive::~KeepAlive(void) {
    if (_e && !_e->isUnboxedVal())
      GC::gc()->removeKeepAlive(this);
  }
  KeepAlive::KeepAlive(const KeepAlive& e) : _e(e._e), _p(NULL), _n(NULL) {
    if (_e && !_e->isUnboxedVal())
      GC::gc()->addKeepAlive(this);
  }
  KeepAlive&
  KeepAlive::operator =(const KeepAlive& e) {
    if (_e && !_e->isUnboxedVal()) {
      if (e._e==NULL || e._e->isUnboxedVal()) {
        GC::gc()->removeKeepAlive(this);
        _p = _n = NULL;
      }
    } else {
      if (e._e!=NULL && !e._e->isUnboxedVal())
        GC::gc()->addKeepAlive(this);
    }
    _e = e._e;
    return *this;
  }

  void
  GC::addWeakRef(WeakRef* e) {
    assert(e->_p==NULL);
    assert(e->_n==NULL);
    e->_n = GC::gc()->_heap->_weakRefs;
    if (GC::gc()->_heap->_weakRefs)
      GC::gc()->_heap->_weakRefs->_p = e;
    GC::gc()->_heap->_weakRefs = e;
  }
  void
  GC::removeWeakRef(MiniZinc::WeakRef *e) {
    if (e->_p) {
      e->_p->_n = e->_n;
    } else {
      assert(GC::gc()->_heap->_weakRefs==e);
      GC::gc()->_heap->_weakRefs = e->_n;
    }
    if (e->_n) {
      e->_n->_p = e->_p;
    }
  }
  void
  GC::addNodeWeakMap(ASTNodeWeakMap* m) {
    assert(m->_p==NULL);
    assert(m->_n==NULL);
    m->_n = GC::gc()->_heap->_nodeWeakMaps;
    if (GC::gc()->_heap->_nodeWeakMaps)
      GC::gc()->_heap->_nodeWeakMaps->_p = m;
    GC::gc()->_heap->_nodeWeakMaps = m;
  }
  void
  GC::removeNodeWeakMap(ASTNodeWeakMap* m) {
    if (m->_p) {
      m->_p->_n = m->_n;
    } else {
      assert(GC::gc()->_heap->_nodeWeakMaps==m);
      GC::gc()->_heap->_nodeWeakMaps = m->_n;
    }
    if (m->_n) {
      m->_n->_p = m->_p;
    }
  }

  WeakRef::WeakRef(Expression* e)
  : _e(e), _p(NULL), _n(NULL), _valid(true) {
    if (_e && !_e->isUnboxedVal())
      GC::gc()->addWeakRef(this);
  }
  WeakRef::~WeakRef(void) {
    if (_e && !_e->isUnboxedVal())
      GC::gc()->removeWeakRef(this);
  }
  WeakRef::WeakRef(const WeakRef& e) : _e(e()), _p(NULL), _n(NULL), _valid(true) {
    if (_e && !_e->isUnboxedVal())
      GC::gc()->addWeakRef(this);
  }
  WeakRef&
  WeakRef::operator =(const WeakRef& e) {
    // Test if this WeakRef is currently active in the GC
    bool isActive = (_e && !_e->isUnboxedVal());
    if (isActive) {
      // Yes, active WeakRef.
      // If after assigning WeakRef should be inactive, remove it.
      if (e()==NULL || e()->isUnboxedVal()) {
        GC::gc()->removeWeakRef(this);
        _n = _p = NULL;
      }
    }
    _e = e();
    _valid = true;
    
    // If this WeakRef was not active but now should be, add it
    if (!isActive && _e!=NULL && !_e->isUnboxedVal())
      GC::gc()->addWeakRef(this);
    return *this;
  }

  ASTNodeWeakMap::ASTNodeWeakMap(void)
  : _p(NULL), _n(NULL) {
    GC::gc()->addNodeWeakMap(this);
  }
  
  ASTNodeWeakMap::~ASTNodeWeakMap(void) {
    GC::gc()->removeNodeWeakMap(this);
  }

  void
  ASTNodeWeakMap::insert(ASTNode* n0, ASTNode* n1) {
    _m.insert(std::make_pair(n0,n1));
  }
  
  ASTNode*
  ASTNodeWeakMap::find(ASTNode* n) {
    NodeMap::iterator it = _m.find(n);
    if (it==_m.end()) return NULL;
    return it->second;
  }
  
}
