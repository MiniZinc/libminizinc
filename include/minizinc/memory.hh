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

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <iosfwd>
#include <new>
#include <type_traits>
#include <utility>
#include <vector>

#if defined(__clang__)
// Lets clang warn (-Wreturn-stack-address) when a local Ref is returned as a raw pointer
#define MZN_LIFETIMEBOUND [[clang::lifetimebound]]
#else
#define MZN_LIFETIMEBOUND
#endif

namespace MiniZinc {

class Expression;

/**
 * \brief Base class for abstract syntax tree nodes (reference counted, see class RC)
 */
class ASTNode {
  friend class RC;
  friend class Expression;

protected:
  /// Reference counts: strong count in the low 24 bits, weak count in the high 8 bits (see class
  /// RC)
  uint32_t _rc = 0;
  /// Id of the node
  unsigned int _id : 7;
  /// Secondary id
  unsigned int _secondaryId : 7;
  /// Flag
  unsigned int _flag1 : 1;
  /// Flag
  unsigned int _flag2 : 1;

  enum BaseNodes { NID_CHUNK, NID_VEC, NID_STR, NID_END = NID_STR };

  /// Constructor
  ASTNode(unsigned int id) : _id(id) {}

public:
  /// Allocate node
  void* operator new(size_t size);

  /// Placement-new
  void* operator new(size_t /*s*/, void* n) throw() { return n; }

  /// Delete node (no-op: see RC::release)
  void operator delete(void* /*n*/, size_t /*s*/) throw() {}
  /// Delete node (no-op)
  void operator delete(void* /*n*/, void* /*m*/) throw() {}

  /// Delete node (no-op)
  void operator delete(void* /*n*/) throw() {}
};

/**
 * \brief Base class for unstructured data nodes
 */
class ASTChunk : public ASTNode {
protected:
  /// Allocated size
  size_t _size;
  /// Storage
  char _data[4];
  /// Constructor
  ASTChunk(size_t size, unsigned int id = ASTNode::NID_CHUNK);
  /// Allocate raw memory
  static void* alloc(size_t size);
};

/**
 * \brief Base class for structured data nodes
 */
class ASTVec : public ASTNode {
  friend class Expression;

protected:
  /// Allocated size
  size_t _size;
  /// Storage
  void* _data[2];
  /// Constructor
  ASTVec(size_t size);
  /// Allocate raw memory
  static void* alloc(size_t size);
};

/// Allocate memory for a node (with mimalloc if MiniZinc is built with MZN_USE_MIMALLOC)
void* node_alloc(size_t size);
/// Free the memory of a node that reference counting has released
void node_free(void* p);
/// Peak memory use of the process in bytes (0 if it is not measured)
size_t peak_memory();

/// The trail, for undoing the bindings of let and comprehension variables
class Trail {
public:
  /// Put a mark on the trail
  static void mark();
  /// Add a trail entry
  static void trail(Expression** l, Expression* v);
  /// Untrail to previous mark
  static void untrail();
  /// Put a mark on the trail, and untrail to it when the scope ends (also on an exception)
  class Scope {
  public:
    Scope() { mark(); }
    ~Scope() { untrail(); }
    Scope(const Scope&) = delete;
    Scope& operator=(const Scope&) = delete;
  };
};

inline void* ASTNode::operator new(size_t size) { return node_alloc(size); }

inline void* ASTVec::alloc(size_t size) {
  return node_alloc(sizeof(ASTVec) + (size <= 2 ? 0 : size - 2) * sizeof(void*));
}

inline void* ASTChunk::alloc(size_t size) {
  return node_alloc(sizeof(ASTChunk) + (size <= 4 ? 0 : size - 4) * sizeof(char));
}

template <class T>
class Ref;
template <class T>
class Weak;

/**
 * \brief Reference counts of AST nodes
 *
 * Every ASTNode has a strong count (24 bits) and a weak count (8 bits), in the style of the
 * control block of std::shared_ptr. Both counts are sticky: a count that reaches its maximum
 * never changes again, so the node is never destroyed (strong) or never freed (weak).
 *
 * - When the strong count reaches 0, the node is destroyed: its children are released.
 * - When the weak count is also 0, its memory goes back to the heap.
 *
 * Pointers to nodes may be unboxed values or carry a tag bit (see Expression::tag). The functions
 * here accept any such pointer and ignore null and unboxed values.
 *
 * Raw pointers to nodes are borrowed: they do not count, and are valid only while a strong
 * reference (Ref, or a counted field of another node) keeps the node alive.
 */
class RC {
public:
  static constexpr uint32_t STRONG_MAX = (1U << 24) - 1;
  static constexpr uint32_t WEAK_ONE = 1U << 24;
  static constexpr uint32_t WEAK_MAX = 0xFFU;
  /// Expression::E_ID and E_VARDECL (checked in memory.cpp)
  static constexpr unsigned int NID_ID = 8;
  static constexpr unsigned int NID_VARDECL = 18;

  /// The node that \a p points to, or nullptr for null and unboxed values
  static ASTNode* node(const void* p) {
    auto u = reinterpret_cast<uintptr_t>(p);
    // Unboxed values use bits 0-1 on 64-bit and bit 0 on 32-bit; tags use the next bit up.
    constexpr uintptr_t unboxed = sizeof(double) <= sizeof(void*) ? 3 : 1;
    constexpr uintptr_t tags = sizeof(double) <= sizeof(void*) ? 7 : 3;
    if ((u & unboxed) != 0) {
      return nullptr;
    }
    return reinterpret_cast<ASTNode*>(u & ~tags);
  }

  static uint32_t strong(const ASTNode* n) { return n->_rc & STRONG_MAX; }
  static uint32_t weak(const ASTNode* n) { return n->_rc >> 24; }
  /// Whether the node has not been destroyed (a variable's own Id lives as long as its VarDecl,
  /// also when nothing holds the Id)
  static bool alive(const void* p) {
    ASTNode* n = node(p);
    return n == nullptr || strong(n) != 0 || (n->_id == NID_ID && ownIdAlive(n));
  }

  static void inc(const void* p) {
    ASTNode* n = node(p);
    if (n != nullptr && strong(n) != STRONG_MAX) {
      if (strong(n) == 0 && n->_id == NID_ID) {
        incOwnDecl(n);  // a variable's own Id holds its VarDecl only while the Id is held
      }
      n->_rc++;
    }
  }
  static void dec(const void* p) {
    ASTNode* n = node(p);
    if (n == nullptr || strong(n) == STRONG_MAX) {
      return;
    }
    assert(strong(n) > 0);
    if ((--n->_rc & STRONG_MAX) == 0) {
      if (ASTNode* d = dying(n)) {
        release(d);
      }
    }
  }
  /// inc for a node that is not an Id and whose pointer has no tag (a string). Unlike inc, it is
  /// small enough to be inlined at every copy of an ASTString.
  static void incPlain(ASTNode* n) {
    if (n != nullptr && strong(n) != STRONG_MAX) {
      n->_rc++;
    }
  }
  /// dec for a node that is not an Id and whose pointer has no tag (see incPlain)
  static void decPlain(ASTNode* n) {
    if (n != nullptr && strong(n) != STRONG_MAX && (--n->_rc & STRONG_MAX) == 0) {
      release(n);
    }
  }
  static void incWeak(const void* p) {
    ASTNode* n = node(p);
    if (n != nullptr && weak(n) != WEAK_MAX) {
      n->_rc += WEAK_ONE;
    }
  }
  static void decWeak(const void* p) {
    ASTNode* n = node(p);
    if (n == nullptr || weak(n) == WEAK_MAX) {
      return;
    }
    assert(weak(n) > 0);
    n->_rc -= WEAK_ONE;
    if (n->_rc == 0) {
      Releasing& r = _r;
      if (r.depth > 0) {
        r.toFree.push_back(n);  // see release
      } else {
        node_free(n);
      }
    }
  }
  /// Store \a v in the strong field \a f (the old value is released after the store, so that a
  /// cascade it starts sees the new state)
  template <class T>
  static void set(T*& f, typename std::common_type<T*>::type v) {  // (not deduced: accepts Arg)
    inc(v);
    T* old = f;
    f = v;
    dec(old);
  }
  /// Set the declaration of Id \a n (counts it as ownDecl describes)
  static void setIdDecl(ASTNode* n, Expression* d);
  /// Store the vector handle \a v (ASTExprVec, ASTIntVec) in the strong field \a f
  template <class H>
  static void setVec(H& f, const H& v) {
    inc(v.vec());
    auto* old = f.vec();
    f = v;
    dec(old);
  }
  /// Store \a v in the weak field \a f
  template <class T>
  static void setWeak(T*& f, typename std::common_type<T*>::type v) {
    incWeak(v);
    T* old = f;
    f = v;
    decWeak(old);
  }
  /// Count the children of the new node \a n (constructors store them without counting)
  template <class T>
  static T* adopt(T* n) {
    adoptChildren(node(n));
    return n;
  }
  /// Rewrite the fields of the complete node \a n with \a write (for example init() after
  /// construction): the new children are counted before the old ones are released.
  template <class F>
  static void replaceChildren(ASTNode* n, F write) {
    // The old children wait on a shared stack (no allocation per call); a nested call only uses
    // the part above base
    size_t base = _oldChildren.size();
    pushChildren(n);
    write();
    adoptChildren(n, true);
    for (size_t i = base; i < _oldChildren.size(); i++) {
      if (_oldChildren[i].second) {
        dec(_oldChildren[i].first);
      } else {
        decWeak(_oldChildren[i].first);
      }
    }
    _oldChildren.resize(base);
  }
  /// Destroy the candidates that are alive only because they hold themselves: a recursive function
  /// through the calls in its body, or a variable through its annotations (`x :: my_ann(x)`).
  /// An environment calls this after it has released the candidates (see Env::~Env). A candidate
  /// that anything else can still reach, and everything that it reaches, does not change.
  static void collectCycles(const std::vector<Weak<ASTNode>>& candidates);
  /// Report counted nodes that no immortal node reaches (only with MZN_RC_CHECK, else no-op)
  static void reportLeaks(std::ostream& os);
  /// Make the node live until the end of the program (used for the Constants table)
  static void immortal(const void* p) {
    ASTNode* n = node(p);
    if (n != nullptr) {
      n->_rc |= STRONG_MAX;
#ifdef MZN_RC_CHECK
      noteImmortal(n);
#endif
    }
  }
#ifdef MZN_RC_CHECK
  /// Record that \a n is immortal on purpose (reportLeaks reports the other saturated counts)
  static void noteImmortal(ASTNode* n);
#endif

private:
  /// Count the children of \a n. The members of a node count its Owned edges themselves when the
  /// node is constructed, so only a rewrite of a complete node counts them (\a owned).
  static void adoptChildren(ASTNode* n, bool owned = false);
  /// Push the children of \a n, and whether each edge is strong, on _oldChildren
  static void pushChildren(ASTNode* n);
  static thread_local std::vector<std::pair<const void*, bool>> _oldChildren;
  /// Destroy \a n and every node that dies with it, without recursion
  static void release(ASTNode* n);
  /// The VarDecl of \a n if \a n is that VarDecl's own Id, else nullptr. A VarDecl holds its own
  /// Id weakly, and the Id holds the VarDecl only while the Id itself is held, so the two do not
  /// form a cycle. The Id dies with the VarDecl.
  static ASTNode* ownDecl(ASTNode* n);
  /// Count the VarDecl of the own Id \a n (not inline, so that inc itself can be inlined)
  static void incOwnDecl(ASTNode* n);
  static bool ownIdAlive(ASTNode* n);
  /// The strong count of \a n has just dropped to 0: the node that dies with that, or nullptr. That
  /// is \a n itself, unless \a n is an own Id: then the Id stays, and its VarDecl loses a count.
  static ASTNode* dying(ASTNode* n);
  /// The state of release. One thread-local object, so that a release looks it up once.
  struct Releasing {
    /// Nesting depth of release calls
    int depth = 0;
    /// Dying nodes whose children are not released yet
    std::vector<ASTNode*> dead;
    /// Destroyed nodes, freed when the outermost release ends
    std::vector<ASTNode*> toFree;
  };
  static thread_local Releasing _r;
};

/// Strong (owning) reference to an AST node
template <class T>
class Ref {
  T* _p = nullptr;

public:
  Ref() = default;
  Ref(T* p) : _p(p) { RC::inc(_p); }
  Ref(const Ref& r) : _p(r._p) { RC::inc(_p); }
  Ref(Ref&& r) noexcept : _p(r._p) { r._p = nullptr; }
  template <class U>
  Ref(const Ref<U>& r) : _p(r.get()) {
    RC::inc(_p);
  }
  template <class U>
  Ref(Ref<U>&& r) noexcept : _p(r.release()) {}
  ~Ref() { RC::dec(_p); }
  Ref& operator=(Ref r) noexcept {
    std::swap(_p, r._p);
    return *this;
  }

  T* get() const& MZN_LIFETIMEBOUND { return _p; }
  /// (a temporary Ref would release the node before the pointer is used)
  T* get() && = delete;
  T* operator->() const { return _p; }
  /// The node as a \a U, like Expression::cast and Item::cast (checked by an assertion)
  template <class U>
  Ref<U> cast() const {
    return Ref<U>(node_cast(_p, static_cast<U*>(nullptr)));
  }
  /// The node as a \a U, or nullptr, like Expression::dynamicCast and Item::dynamicCast
  template <class U>
  Ref<U> dynamicCast() const {
    return Ref<U>(node_dynamic_cast(_p, static_cast<U*>(nullptr)));
  }
  T& operator*() const { return *_p; }
  // A temporary Ref may be the only owner, so it must not decay to a raw pointer.
  operator T*() const& MZN_LIFETIMEBOUND { return _p; }
  operator T*() && = delete;
  // Comparing a temporary with nullptr is safe
  friend bool operator==(const Ref& r, std::nullptr_t) { return r._p == nullptr; }
  friend bool operator!=(const Ref& r, std::nullptr_t) { return r._p != nullptr; }
  friend bool operator==(std::nullptr_t, const Ref& r) { return r._p == nullptr; }
  friend bool operator!=(std::nullptr_t, const Ref& r) { return r._p != nullptr; }
  /// Give up ownership without a decrement (the caller now owns one count)
  T* release() {
    T* p = _p;
    _p = nullptr;
    return p;
  }
};

/**
 * \brief Parameter type for functions that count the pointer they store (setters)
 *
 * It accepts raw pointers and Refs, also temporary ones: a temporary lives until the end of the
 * full expression, which is longer than the call that counts it.
 */
template <class T>
class Arg {
  T* _p;

public:
  Arg(T* p) : _p(p) {}
  /// Anything else that reads as a T*, such as an element of an ASTExprVec
  template <class U,
            class = typename std::enable_if<std::is_convertible<const U&, T*>::value>::type>
  Arg(const U& u) : _p(u) {}
  template <class U>
  Arg(const Ref<U>& r) : _p(r.get()) {}
  operator T*() const { return _p; }
  T* operator->() const { return _p; }
};

/// Owns nodes for holders that cannot hold a Ref, such as the value stacks of the parsers, until it
/// is cleared or destroyed
class NodeOwner {
  std::vector<Ref<ASTNode>> _nodes;

public:
  /// Own \a r, and return the node for the holder
  template <class T>
  T* keep(Ref<T> r) {
    T* p = r.get();
    if (RC::node(p) != nullptr) {  // (unboxed values need no owner)
      _nodes.emplace_back(std::move(r));
    }
    return p;
  }
  void clear() { _nodes.clear(); }
};

/// Make a new node live until the end of the program (for static tables such as Constants)
template <class T>
T* immortal(Ref<T>&& r) {
  RC::immortal(r.get());
  return r.get();
}

/// The raw pointers of a vector of references, for functions that take std::vector<T*>
template <class T>
std::vector<T*> raw(const std::vector<Ref<T>>& v) {
  std::vector<T*> r;
  r.reserve(v.size());
  for (const auto& e : v) {
    r.push_back(e.get());
  }
  return r;
}

namespace RCDetail {
template <class A>
A&& arg(A&& a) {
  return std::forward<A>(a);
}
// A temporary Ref argument lives until the end of the full expression that calls make, which is
// longer than the constructor that takes (and counts) the pointer.
template <class U>
U* arg(Ref<U>&& r) {
  return r.get();
}
template <class U>
std::vector<U*> arg(const std::vector<Ref<U>>& v) {
  return raw(v);
}
template <class U>
std::vector<U*> arg(std::vector<Ref<U>>& v) {
  return raw(v);
}
template <class U>
std::vector<U*> arg(std::vector<Ref<U>>&& v) {
  return raw(v);
}
}  // namespace RCDetail

/// Allocate a new node, owned by the returned reference
template <class T, class... Args>
Ref<T> make(Args&&... args) {
  return Ref<T>(RC::adopt(new T(RCDetail::arg(std::forward<Args>(args))...)));
}

/// Weak reference to an AST node: keeps the memory, but not the node, alive
template <class T>
class Weak {
  T* _p = nullptr;

public:
  Weak() = default;
  Weak(T* p) : _p(p) { RC::incWeak(_p); }
  Weak(const Weak& w) : _p(w._p) { RC::incWeak(_p); }
  Weak(Weak&& w) noexcept : _p(w._p) { w._p = nullptr; }
  ~Weak() { RC::decWeak(_p); }
  Weak& operator=(Weak w) noexcept {
    std::swap(_p, w._p);
    return *this;
  }

  /// The node, or nullptr if it has been destroyed
  T* get() const { return RC::alive(_p) ? _p : nullptr; }
  /// Whether this reference was never set (as opposed to pointing at a destroyed node)
  bool isNull() const { return _p == nullptr; }
};

}  // namespace MiniZinc
