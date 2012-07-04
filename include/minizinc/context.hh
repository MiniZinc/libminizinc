#ifndef __MINIZINC_CONTEXT_HH__
#define __MINIZINC_CONTEXT_HH__

#include <minizinc/allocator.hh>

#include <string>
#include <vector>
#include <cstring>

namespace MiniZinc {

  /**
   * \brief Context for AST operations
   *
   * Provides allocator for AST nodes, as well as symbol and hash tables
   *
   */
  class ASTContext {
  protected:
    mutable BlockAllocator balloc;
  public:
    void* alloc(size_t size) const {
      return balloc.alloc(size);
    }
    template<typename T> T* alloc(void) const {
      return balloc.alloc<T>();
    }
    template<typename T> T* alloc(int n) const {
      return balloc.alloc<T>(n);
    }
    void dealloc(const void* m) { (void) m; }
  
    ~ASTContext(void) {}
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
  };

  /**
   * \brief Context-allocated string
   */
  class CtxString {
  protected:
    unsigned int _n;
    char _s[1];
  public:
    static CtxString* a(const ASTContext& ctx, const std::string& s) {
      CtxString* cs = static_cast<CtxString*>(
        ctx.alloc(sizeof(CtxString)+(s.size())));
      cs->_n = s.size();
      strncpy(cs->_s,s.c_str(),s.size()+1);
      return cs;
    }
    const char* c_str(void) const { return _s; }
    std::string str(void) const { return std::string(c_str()); }
    unsigned int size(void) const { return _n; }
  };

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
    explicit CtxStringH(CtxString* s) : _s(s) {}
    /// Copy constructor
    CtxStringH(const CtxStringH& s) : _s(s._s) {}
    /// Assignment operator
    CtxStringH& operator= (const CtxStringH& s) {
      if (this != &s) {
        _s = s._s;
      }
      return *this;
    }

    unsigned int size(void) const {
      return _s ? _s->size() : 0;
    }

    bool operator== (const CtxStringH& s) const {
      return size()==s.size() &&
        (size()==0 || strncmp(_s->c_str(),s._s->c_str(),size())==0);
    }
  };

}

#endif
