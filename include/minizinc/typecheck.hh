#ifndef __MINIZINC_TYPECHECK_HH__
#define __MINIZINC_TYPECHECK_HH__

#include <minizinc/model.hh>

namespace MiniZinc {

  void addOperatorTypes(ASTContext& ctx);
  
  void typecheck(ASTContext& ctx, Model* m);
  
}

#endif
