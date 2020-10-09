/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <minizinc/config.hh>
#include <minizinc/timer.hh>

#include <cassert>
#include <cstdlib>
#include <new>
#include <unordered_map>

//#define MINIZINC_GC_STATS

#if defined(MINIZINC_GC_STATS)
#include <map>

struct GCStat {
  int first;
  int second;
  int keepalive;
  int inmodel;
  size_t total;
  GCStat() : first(0), second(0), keepalive(0), inmodel(0), total(0) {}
};

#define MINIZINC_GC_STAT_ARGS std::map<int, GCStat>& gc_stats
#else

#define MINIZINC_GC_STAT_ARGS

#endif

namespace MiniZinc {

/**
 * \brief Base class for abstract syntax tree nodes
 */
class ASTNode {
  friend class GC;

protected:
  /// Mark for garbage collection
  mutable unsigned int _gcMark : 1;
  /// Id of the node
  unsigned int _id : 7;
  /// Secondary id
  unsigned int _secondaryId : 7;
  /// Flag
  unsigned int _flag1 : 1;
  /// Flag
  unsigned int _flag2 : 1;

  enum BaseNodes { NID_FL, NID_CHUNK, NID_VEC, NID_STR, NID_END = NID_STR };

  /// Constructor
  ASTNode(unsigned int id) : _gcMark(0), _id(id) {}

public:
  /// Allocate node
  void* operator new(size_t size);

  /// Placement-new
  void* operator new(size_t /*s*/, void* n) throw() { return n; }

  /// Delete node (no-op)
  void operator delete(void* /*n*/, size_t /*s*/) throw() {}
  /// Delete node (no-op)
  void operator delete(void* /*n*/, void* /*m*/) throw() {}

  /// Delete node (no-op)
  void operator delete(void* /*n*/) throw() {}
};

/**
 * \brief Base class for unstructured garbage collected data
 */
class ASTChunk : public ASTNode {
  friend class GC;

protected:
  /// Allocated size
  size_t _size;
  /// Storage
  char _data[4];
  /// Constructor
  ASTChunk(size_t size, unsigned int id = ASTNode::NID_CHUNK);
  /// Actual size of object in memory
  size_t memsize() const {
    size_t s = sizeof(ASTChunk) + (_size <= 4 ? 0 : _size - 4) * sizeof(char);
    s += ((8 - (s & 7)) & 7);
    return s;
  }
  /// Allocate raw memory
  static void* alloc(size_t size);
};

/**
 * \brief Base class for structured garbage collected data
 */
class ASTVec : public ASTNode {
  friend class GC;

protected:
  /// Allocated size
  size_t _size;
  /// Storage
  void* _data[2];
  /// Constructor
  ASTVec(size_t size);
  /// Actual size of object in memory
  size_t memsize() const {
    size_t s = sizeof(ASTVec) + (_size <= 2 ? 0 : _size - 2) * sizeof(void*);
    s += ((8 - (s & 7)) & 7);
    return s;
  }
  /// Allocate raw memory
  static void* alloc(size_t size);
};

class Expression;

class GCMarker;
class KeepAlive;
class WeakRef;

class ASTNodeWeakMap;
class ASTStringData;

/// Garbage collector
class GC {
  friend class ASTNode;
  friend class ASTVec;
  friend class ASTChunk;
  friend class ASTStringData;
  friend class KeepAlive;
  friend class WeakRef;
  friend class ASTNodeWeakMap;

private:
  class Heap;
  /// The memory controlled by the collector
  Heap* _heap;
  /// Count how many locks are currently active
  unsigned int _lockCount;
  /// Timeout in milliseconds
  unsigned long long int _timeout;
  /// Counter for timeout
  int _timeoutCount;
  /// Timer for timeout
  Timer _timeoutTimer;
  /// Return thread-local GC object
  static GC*& gc();
  /// Constructor
  GC();

  /// Allocate garbage collected memory
  void* alloc(size_t size);

  static void addKeepAlive(KeepAlive* e);
  static void removeKeepAlive(KeepAlive* e);
  static void addWeakRef(WeakRef* e);
  static void removeWeakRef(WeakRef* e);
  static void addNodeWeakMap(ASTNodeWeakMap* m);
  static void removeNodeWeakMap(ASTNodeWeakMap* m);

public:
  /// Acquire garbage collector lock for this thread
  static void lock();
  /// Release garbage collector lock for this thread
  static void unlock();
  /// Manually trigger garbage collector (must be unlocked)
  static void trigger();
  /// Test if garbage collector is locked
  static bool locked();
  /// Add model \a m to root set
  static void add(GCMarker* m);
  /// Remove model \a m from root set
  static void remove(GCMarker* m);

  /// Put a mark on the trail
  static void mark();
  /// Add a trail entry
  static void trail(Expression** l, Expression* v);
  /// Untrail to previous mark
  static void untrail();

  /// Set timeout of \a t milliseconds, 0 means disable
  static void setTimeout(unsigned long long int t);

  /// Return maximum allocated memory (high water mark)
  static size_t maxMem();
};

/// Automatic garbage collection lock
class GCLock {
public:
  /// Acquire lock
  GCLock();
  /// Release lock upon destruction
  ~GCLock();
};

/// Expression wrapper that is a member of the root set
class KeepAlive {
  friend class GC;

private:
  Expression* _e;
  KeepAlive* _p;
  KeepAlive* _n;

public:
  KeepAlive(Expression* e = nullptr);
  ~KeepAlive();
  KeepAlive(const KeepAlive& e);
  KeepAlive& operator=(const KeepAlive& e);
  Expression* operator()() { return _e; }
  Expression* operator()() const { return _e; }
  KeepAlive* next() const { return _n; }
};

/// Expression wrapper that is a member of the root set
class WeakRef {
  friend class GC;

private:
  Expression* _e;
  WeakRef* _p;
  WeakRef* _n;
  bool _valid;

public:
  WeakRef(Expression* e = nullptr);
  ~WeakRef();
  WeakRef(const WeakRef& e);
  WeakRef& operator=(const WeakRef& e);
  Expression* operator()() { return _valid ? _e : nullptr; }
  Expression* operator()() const { return _valid ? _e : nullptr; }
  WeakRef* next() const { return _n; }
};

class ASTNodeWeakMap {
  friend class GC;

private:
  ASTNodeWeakMap(const WeakRef& e);
  ASTNodeWeakMap& operator=(const ASTNodeWeakMap& e);

protected:
  typedef std::unordered_map<ASTNode*, ASTNode*> NodeMap;
  ASTNodeWeakMap* _p;
  ASTNodeWeakMap* _n;
  ASTNodeWeakMap* next() const { return _n; }
  NodeMap _m;

public:
  ASTNodeWeakMap();
  ~ASTNodeWeakMap();
  void insert(ASTNode* n0, ASTNode* n1);
  ASTNode* find(ASTNode* n);
  void clear() { _m.clear(); }
};

/**
 * \brief Abstract base class for object containing garbage collected data
 */
class GCMarker {
  friend class GC;

private:
  /// Previous object in root set list
  GCMarker* _rootsPrev = nullptr;
  /// Next object in root set list
  GCMarker* _rootsNext = nullptr;

protected:
  /// Mark garbage collected objects that
  virtual void mark(MINIZINC_GC_STAT_ARGS) = 0;

public:
  GCMarker() { GC::add(this); }
  virtual ~GCMarker() { GC::remove(this); }
};

}  // namespace MiniZinc
