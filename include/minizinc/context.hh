#ifndef __MINIZINC_CONTEXT_HH__
#define __MINIZINC_CONTEXT_HH__

#include <minizinc/allocator.hh>

#include <string>
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
  
  char* alloc(const std::string& s) const {
    char* c = alloc<char>(s.size()+1);
    return strncpy(c,s.c_str(),s.size()+1);
  }
  
  ~ASTContext(void) {}
};

}

#endif
