#ifndef __SOLVER_INTERFACE_H
#define __SOLVER_INTERFACE_H

#include <map>
#include <minizinc/ast.hh>
#include <minizinc/parser.hh>
#include <minizinc/model.hh>
#include <minizinc/printer.h>
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
      if(e->isa<BinOp>()){
	BinOp* bo = e->cast<BinOp>();
	double b, u;
	b = getNumber<double,FloatLit>(bo->_e0);
	u = getNumber<double,FloatLit>(bo->_e1);
	return std::pair<double,double>(b,u);
      }
      else {
	std::cerr << "getFloatBounds : Expected BinOp, got this : " << e->_eid;
	Printer::getInstance()->print(e);
	std::exit(0);
      }
    }
    static std::pair<double,double> getIntBounds(Expression* e){
      if(e->isa<BinOp>()){
	BinOp* bo = e->cast<BinOp>();
	int b, u;
	b = getNumber<int,IntLit>(bo->_e0);
	u = getNumber<int,IntLit>(bo->_e1);
	return std::pair<int,int>(b,u);
      }
      else {
	return getIntBounds(e->cast<TypeInst>()->_domain);
	std::cerr << "getIntBounds : Expected BinOp, got this : " << printEID(e->_eid);
	Printer::getInstance()->print(e);
	std::exit(0);
      }
    }
    poster lookupConstraint(std::string& s);
    std::map<VarDecl*, void*> variableMap;
    std::map<std::string, poster> constraintMap;
    /*enum ExpressionId {

      } _eid;*/

    static std::string printEID(Expression::ExpressionId eid){
      switch(eid){
      case Expression::ExpressionId::E_INTLIT: return "INTLIT";
      case Expression::ExpressionId::E_FLOATLIT: return "FLOATLIT"; 
      case Expression::ExpressionId::E_SETLIT: return "SETLIT"; 
      case Expression::ExpressionId::E_BOOLLIT: return "BOOLLIT";
      case Expression::ExpressionId::E_STRINGLIT: return "STRINGLIT";
      case Expression::ExpressionId::E_ID: return "ID";
      case Expression::ExpressionId::E_ANON: return "ANON";
      case Expression::ExpressionId::E_ARRAYLIT: return "ARRAYLIT";
      case Expression::ExpressionId::E_ARRAYACCESS: return "ARRAYACCESS";
      case Expression::ExpressionId::E_COMP: return "COMP";
      case Expression::ExpressionId::E_ITE: return "ITE";
      case Expression::ExpressionId::E_BINOP: return "BINOP"; 
      case Expression::ExpressionId::E_UNOP: return "UNOP"; 
      case Expression::ExpressionId::E_CALL: return "CALL";
      case Expression::ExpressionId::E_VARDECL: return "VARDECL"; 
      case Expression::ExpressionId::E_LET: return "LET";
      case Expression::ExpressionId::E_ANN: return "ANN"; 
      case Expression::ExpressionId::E_TI: return "TI"; 
      case Expression::ExpressionId::E_TIID: return "TIID";
      }
    }
  };
};
#endif
