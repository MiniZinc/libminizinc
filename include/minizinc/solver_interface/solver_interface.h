#ifndef __SOLVER_INTERFACE_H
#define __SOLVER_INTERFACE_H

#include <map>
#include <minizinc/ast.hh>
#include <minizinc/parser.hh>
#include <minizinc/model.hh>
namespace MiniZinc{
  template<typename T, typename S>
    T getNumber(Expression* e) {
    if (e->isa<S>())
      return e->cast<S>()->_v;
    if (e->isa<UnOp>())
      return getNumber<T, S>(e->cast<UnOp>()->_e0)
	* (e->cast<UnOp>()->_op == UOT_MINUS ? -1 : 1);
    return 0;
  }
  class SolverInterface {
  public:
    SolverInterface();
    virtual ~SolverInterface();

    virtual void* getModel()=0;
    virtual void solve(SolveI* s) = 0;
    void addVar(VarDecl* vd);
    void postConstraint(ConstraintI& constraint);
    virtual void* resolveVar(Expression*)=0;
	
    void fromFlatZinc(MiniZinc::Model& m);
    void* lookupVar(VarDecl* vd);
    void* lookupVar(std::string s);
  protected:
    virtual void* addSolverVar(VarDecl*) = 0;
   

    typedef void (*poster) (SolverInterface&, const CtxVec<Expression*>&);
    
    void addConstraintMapping(std::string mzn_constraint,
			      poster func);
    static std::pair<double,double> getFloatBounds(Expression* e){
      BinOp* bo = e->cast<BinOp>();
      double b, u;
      b = getNumber<double,FloatLit>(bo->_e0);
      u = getNumber<double,FloatLit>(bo->_e1);
      return std::pair<double,double>(b,u);
    }
    static std::pair<double,double> getIntBounds(Expression* e){
      BinOp* bo = e->cast<BinOp>();
      int b, u;
      b = getNumber<int,IntLit>(bo->_e0);
      u = getNumber<int,IntLit>(bo->_e1);
      return std::pair<int,int>(b,u);
    }
    poster lookupConstraint(std::string& s);
    std::map<VarDecl*, void*> variableMap;
    std::map<std::string, poster> constraintMap;
    
  };
};
#endif
