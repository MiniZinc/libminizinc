/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/gc.hh>

#include <vector>

namespace MiniZinc {
  
  GC*&
  GC::gc(void) {
    static __thread GC* gc = NULL;
    return gc;
  }
  
  void
  GC::init(void) {
    if (gc()==NULL) {
      gc() = new GC();
    }
  }
  
  bool
  GC::locked(void) {
    assert(gc());
    return gc()->_lock_count > 0;
  }

  void
  GC::lock(void) {
    assert(gc());
    gc()->_lock_count++;
  }
  void
  GC::unlock(void) {
    assert(locked());
    gc()->_lock_count--;
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
  };

  class HeapPage {
  public:
    HeapPage* _next;
    size_t size;
    char data[1];
  };

  /// Memory managed by the garbage collector
  class GC::Heap {
    friend class GC;
  protected:
    ASTChunk* _chunks;
    ASTVec* _vecs;
    HeapPage* _pages;
    static const int _max_fl = 5;
    FreeListNode* _fl[_max_fl+1];
    static const size_t _fl_size[_max_fl+1];
    void fill_fl(int slot) {}
    int _fl_slot(size_t size) {
      assert(size <= _fl_size[_max_fl]);
      assert(size >= sizeof(ASTNode));
      size -= sizeof(ASTNode);
      size /= sizeof(void*);
      int slot = static_cast<int>(size)-1;
      return slot < 0 ? 0 : slot;
    }

    /// A trail item
    struct TItem {
      void** l;
      void* v;
      bool mark;
      TItem(void** l0, void* v0)
        : l(l0), v(v0), mark(false) {}
    };
    /// Trail
    std::vector<TItem> trail;

  };

  const size_t
  GC::Heap::_fl_size[GC::Heap::_max_fl+1] = {
    sizeof(ASTNode)+1*sizeof(void*),
    sizeof(ASTNode)+2*sizeof(void*),
    sizeof(ASTNode)+3*sizeof(void*),
    sizeof(ASTNode)+4*sizeof(void*),
    sizeof(ASTNode)+5*sizeof(void*),
    sizeof(ASTNode)+6*sizeof(void*),
  };

  GC::GC(void) : _heap(new Heap()), _lock_count(0) {}

  void
  GC::_registerChunk(ASTChunk* c) {
    if (c->memsize() > _heap->_fl_size[_heap->_max_fl]) {
      c->_next = _heap->_chunks;
      _heap->_chunks = c;
    }
  }
  void
  GC::_registerVec(ASTVec* v) {
    if (v->memsize() > _heap->_fl_size[_heap->_max_fl]) {
      v->_next = _heap->_vecs;
      _heap->_vecs = v;
    }
  }

  void
  GC::add(Model* m) {
    /// TODO
  }

  void
  GC::remove(Model* m) {
    /// TODO
  }

  void*
  GC::alloc(size_t size) {
    return ::malloc(size); /// TODO
    if (size > _heap->_fl_size[_heap->_max_fl]) {
      return ::malloc(size);
    } else {
      int slot = _heap->_fl_slot(size);
      _heap->fill_fl(slot);
      FreeListNode* p = _heap->_fl[slot];
      _heap->_fl[slot] = p->next;
      return p;
    }
  }

  ASTVec::ASTVec(size_t size)
    : ASTNode(NID_VEC), _size(size) {
    GC::gc()->_registerVec(this);
  }
  void*
  ASTVec::alloc(size_t size) {
    return GC::gc()->alloc(size);
  }

  ASTChunk::ASTChunk(size_t size)
    : ASTNode(NID_CHUNK), _size(size) {
    GC::gc()->_registerChunk(this);
  }
  void*
  ASTChunk::alloc(size_t size) {
    return GC::gc()->alloc(size);
  }

  void
  GC::mark(void) {
    GC* gc = GC::gc();
    if (!gc->_heap->trail.empty())
      gc->_heap->trail.back().mark = true;
  }
  void
  GC::trail(void** l,void* v) {
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

  void*
  ASTNode::operator new(size_t size) throw (std::bad_alloc) {
    return GC::gc()->alloc(size);
  }

}
