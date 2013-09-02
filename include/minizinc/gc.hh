/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MINIZINC_GC_HH__
#define __MINIZINC_GC_HH__

#include <cstdlib>
#include <cassert>
#include <new>

namespace MiniZinc {
  
  /**
   * \brief Base class for abstract syntax tree nodes
   */
  class ASTNode {
    friend class GC;
  protected:
    /// Mark for garbage collection
    unsigned int _gc_mark : 1;
    /// Id of the node
    unsigned int _id : 7;
    /// Secondary id
    unsigned int _sec_id : 7;
    /// Flag
    unsigned int _flag_1 : 1;
    /// Flag
    unsigned int _flag_2 : 1;
    
    enum BaseNodes { NID_FL, NID_CHUNK, NID_VEC, NID_END = NID_VEC };

    /// Constructor
    ASTNode(unsigned int id) : _gc_mark(0), _id(id) {}

    /// Allocate node
    void* operator new(size_t size) throw (std::bad_alloc);

    /// Placement-new
    void* operator new(size_t, void* n) throw() {
      return n;
    }

    /// Delete node (no-op)
    void operator delete(void*, size_t) throw() { }
    /// Delete node (no-op)
    void operator delete(void*, void*) throw() { }

    /// Delete node (no-op)
    void operator delete(void*) throw() {}
  };
    
  /**
   * \brief Base class for unstructured garbage collected data
   */
  class ASTChunk : public ASTNode {
    friend class GC;
  protected:
    size_t _size;
    char _data[4];
    ASTChunk(size_t size);
    size_t memsize(void) const {
      size_t s = sizeof(ASTChunk)+(_size<=4?0:_size-4)*sizeof(char);
      s += ((8 - (s & 7)) & 7);
      return s;
    }
    static void* alloc(size_t size);
  };


  /**
   * \brief Base class for structured garbage collected data
   */
  class ASTVec : public ASTNode {
    friend class GC;
  protected:
    size_t _size;
    void* _data[2];
    ASTVec(size_t size);
    size_t memsize(void) const {
      size_t s = sizeof(ASTVec)+(_size<=2?0:_size-2)*sizeof(void*);
      s += ((8 - (s & 7)) & 7);
      return s;
    }
    static void* alloc(size_t size);
  };

  class Model;
  class Expression;

  class ASTRootSetIter {
  public:
    virtual Expression** begin(void) = 0;
    virtual Expression** end(void) = 0;
    virtual ~ASTRootSetIter(void) {}
  };
  class ASTRootSet {
  public:
    virtual ASTRootSetIter* rootSet(void) = 0;
  };

  /// Garbage collector
  class GC {
    friend class ASTNode;
    friend class ASTVec;
    friend class ASTChunk;
  private:
    class Heap;
    /// The memory controlled by the collector
    Heap* _heap;
    /// Count how many locks are currently active
    unsigned int _lock_count;
    /// Return thread-local GC object
    static GC*& gc(void);
    /// Constructor
    GC(void);

    /// Allocate garbage collected memory
    void* alloc(size_t size);

  public:
    /**
     * \brief Initialize thread-local GC object
     *
     * Must be called at least once per thread
     */
    static void init(void);
    /// Acquire garbage collector lock for this thread
    static void lock(void);
    /// Release garbage collector lock for this thread
    static void unlock(void);
    /// Test if garbage collector is locked
    static bool locked(void);
    /// Add model \a m to root set
    static void add(Model* m);
    /// Remove model \a m from root set
    static void remove(Model* m);
    
    /// Put a mark on the trail
    static void mark(void);
    /// Add a trail entry
    static void trail(void**,void*);
    /// Untrail to previous mark
    static void untrail(void);
    
    /// Add external root set
    static void addRootSet(ASTRootSet* rs);
    /// Remove external root set
    static void removeRootSet(ASTRootSet* rs);
  };

  /// Automatic garbage collection lock
  class GCLock {
  public:
    /// Acquire lock
    GCLock(void);
    /// Release lock upon destruction
    ~GCLock(void);
  };

}

#endif
