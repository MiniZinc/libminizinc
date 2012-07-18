#ifndef __MINIZINC_TYPECHECK_HH__
#define __MINIZINC_TYPECHECK_HH__

#include <minizinc/model.hh>

namespace MiniZinc {
  
  void typecheck(ASTContext& ctx, Model* m);
  
}

#endif
