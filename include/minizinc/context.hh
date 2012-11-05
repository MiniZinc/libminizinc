/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MINIZINC_CONTEXT_HH__
#define __MINIZINC_CONTEXT_HH__

#include <minizinc/allocator.hh>
#include <minizinc/type.hh>

#include <string>
#include <vector>
#include <cstring>
#include <functional>
#include <unordered_map>

namespace MiniZinc {

  class CtxString;
  class ASTContext;
  
  /**
   * \brief Handler for CtxString objects
   */
  class CtxStringH {
  protected:
    /// String
    CtxString* _s;
  public:
    /// Default constructor
    CtxStringH(void) : _s(NULL) {}
    /// Constructor
    CtxStringH(CtxString* s) : _s(s) {}
    /// Constructor
    CtxStringH(const ASTContext& ctx, const std::string& s);
    /// Copy constructor
    CtxStringH(const CtxStringH& s);
    /// Assignment operator
    CtxStringH& operator= (const CtxStringH& s);

    unsigned int size(void) const;
    const char* c_str(void) const;
    std::string str(void) const;
    CtxString* ctxstr(void) const { return _s; }

    bool operator== (const CtxStringH& s) const;
    bool operator!= (const CtxStringH& s) const;

    bool operator== (const std::string& s) const;
    bool operator!= (const std::string& s) const;
    
    size_t hash(void) const;
    
  };

  class FunctionI;
  class Expression;

}

namespace std {
  template<>
  struct hash<MiniZinc::CtxStringH> {
  public:
    size_t operator()(const MiniZinc::CtxStringH& s) const;
  };
  template<>
  struct equal_to<MiniZinc::CtxStringH> {
  public:
    bool operator()(const MiniZinc::CtxStringH& s0,
                    const MiniZinc::CtxStringH& s1) const;
  };
}

namespace MiniZinc {

  template<typename T>
  struct CtxStringMap {
    typedef std::unordered_map<CtxStringH,T> t;
  };
  
  class VarDecl;
  class Expression;

  /**
   * \brief Context for AST operations
   *
   * Provides allocator for AST nodes, as well as symbol and hash tables
   *
   */
  class ASTContext {
  protected:
    mutable std::vector<BlockAllocator> balloc;
    std::vector<int> cur_balloc;
    
    typedef CtxStringMap<std::vector<FunctionI*> >::t FnMap;
    FnMap fnmap;

    /// A trail item representing a VarDecl assignment
    struct TItem {
      VarDecl* v;
      Expression* e;
      bool mark;
      TItem(VarDecl* v0, Expression* e0)
        : v(v0), e(e0), mark(false) {}
    };
    /// Trail of VarDecl assignments
    std::vector<TItem> vdtrail;
  public:
    void* alloc(size_t size) const {
      return balloc[cur_balloc.back()].alloc(size);
    }
    template<typename T> T* alloc(void) const {
      return balloc[cur_balloc.back()].alloc<T>();
    }
    template<typename T> T* alloc(int n) const {
      return balloc[cur_balloc.back()].alloc<T>(n);
    }
    void dealloc(const void* m) { (void) m; }
    
    ASTContext(void) {
      balloc.push_back(BlockAllocator());
      cur_balloc.push_back(0);
    }
    
    ~ASTContext(void) {
      assert(vdtrail.empty());
    }
    
    void registerFn(FunctionI* fi);
    void sortFn(void);
    FunctionI* matchFn(const CtxStringH&id,
                       const std::vector<Expression*>& args) const;
    FunctionI* matchFn(const CtxStringH& id, const std::vector<Type>& t);

    void trail(VarDecl* v);
    void mark(void);
    void untrail(void);

    void push_allocator(int a);
    void pop_allocator(void);
  };

  /**
   * \brief Context-allocated vector
   */
  template<class T>
  class CtxVec {
  public:
    /// Size of the vector
    unsigned int _n;
    /// Elements
    T _v[1];
  protected:
    /// Constructor, initialising from STL vector \a v
    CtxVec(const std::vector<T>& v) : _n(v.size()) {
      for (unsigned int i=_n; i--;)
        _v[i]=v[i];
    }
  public:
    /// Allocate from given vector \a x in context \a ctx
    static CtxVec* a(const ASTContext& ctx, const std::vector<T>& x) {
      CtxVec<T>* v = static_cast<CtxVec<T>*>(
        ctx.alloc(sizeof(CtxVec<T>)+(x.size()-1)*sizeof(T)));
      new (v) CtxVec<T>(x);
      return v;
    }

    /// Allocate empty vector in context \a ctx
    static CtxVec* a(const ASTContext& ctx) {
      CtxVec<T>* v = static_cast<CtxVec<T>*>(
        ctx.alloc(sizeof(CtxVec<T>)));
      new (v) CtxVec<T>(std::vector<T>());
      return v;
    }

    /// Test if vector is empty
    bool empty(void) const { return _n==0; }
    
    /// Return size of vector
    unsigned int size(void) const { return _n; }
    
    /// Element access
    T& operator[] (int i) {
      assert(i<static_cast<int>(_n)); return _v[i];
    }
    /// Element access
    const T& operator[] (int i) const {
      assert(i<static_cast<int>(_n)); return _v[i];
    }
    
    T* begin(void) { return &_v[0]; }
    T* end(void) { return &_v[_n]; }
  };

  /**
   * \brief Context-allocated string
   */
  class CtxString {
  protected:
    /// Hash value
    size_t _hash;
    /// String length
    unsigned int _n;
    /// Character data
    char _s[1];
  public:
    static CtxString* a(const ASTContext& ctx, const std::string& s) {
      CtxString* cs = static_cast<CtxString*>(
        ctx.alloc(sizeof(CtxString)+(s.size())));
      cs->_n = s.size();
      strncpy(cs->_s,s.c_str(),s.size()+1);
      std::hash<std::string> h;
      cs->_hash = h(s);
      return cs;
    }
    const char* c_str(void) const { return _s; }
    std::string str(void) const { return std::string(c_str()); }
    unsigned int size(void) const { return _n; }
    char operator[](unsigned int i) { assert(i<_n); return _s[i]; }
    size_t hash(void) const { return _hash; }
  };

  inline
  CtxStringH::CtxStringH(const ASTContext& ctx, const std::string& s)
    : _s(CtxString::a(ctx,s)) {}
  inline
  CtxStringH::CtxStringH(const CtxStringH& s) : _s(s._s) {}
  inline CtxStringH&
  CtxStringH::operator= (const CtxStringH& s) {
    if (this != &s) {
      _s = s._s;
    }
    return *this;
  }
  inline unsigned int
  CtxStringH::size(void) const {
    return _s ? _s->size() : 0;
  }
  inline const char*
  CtxStringH::c_str(void) const { return _s ? _s->c_str() : NULL; }
  inline std::string
  CtxStringH::str(void) const { return _s ? _s->str() : std::string(""); }

  inline bool
  CtxStringH::operator== (const CtxStringH& s) const {
    return size()==s.size() &&
      (size()==0 || strncmp(_s->c_str(),s._s->c_str(),size())==0);
  }
  inline bool
  CtxStringH::operator!= (const CtxStringH& s) const {
    return !(*this == s);
  }
  inline bool
  CtxStringH::operator== (const std::string& s) const {
    return size()==s.size() &&
      (size()==0 || strncmp(_s->c_str(),s.c_str(),size())==0);
  }
  inline bool
  CtxStringH::operator!= (const std::string& s) const {
    return !(*this == s);
  }
  
  inline size_t
  CtxStringH::hash(void) const {
    return _s ? _s->hash() : 0;
  }

}

namespace std {
  inline size_t
  hash<MiniZinc::CtxStringH>::operator()(
      const MiniZinc::CtxStringH& s) const {
      return s.hash();
  }
  inline bool
  equal_to<MiniZinc::CtxStringH>::operator()(const MiniZinc::CtxStringH& s0,
                                             const MiniZinc::CtxStringH& s1) 
                                             const {
      return s0==s1;
  }
}

#endif
