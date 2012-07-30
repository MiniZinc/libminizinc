#ifndef __SOLVER_INTERFACE_H
#define __SOLVER_INTERFACE_H

#include <map>
#include <minizinc/ast.hh>
#include <minizinc/parser.hh>
#include <minizinc/model.hh>
#include <minizinc/printer.hh>
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
  protected:
    bool free;
    bool allSolutions;
    bool nbThreads;
  public:
    SolverInterface() : free(false), allSolutions(false), nbThreads(1) {}
    virtual ~SolverInterface();
    void setFree(bool f) { free = f; }
    void setAllSolutions(bool as) { allSolutions = as; }
    void setNbThreads(int n) { nbThreads = n; }
    /* virtual void* fromFlatZinc(Model*); */
    virtual void* getModel()=0;
    virtual void solve(SolveI* s) = 0;
    virtual void* resolveArrayAccess(void* array, int index)=0;
    virtual void* resolveIntLit(int v)=0;
    virtual void* resolveBoolLit(bool v)=0;
    virtual void* resolveFloatLit(double v)=0;
    virtual void* resolveArrayLit(ArrayLit* a)=0;

    void addVar(VarDecl* vd);
    void addVar(VarDecl* vd, void* ptr);
    void postConstraint(ConstraintI& constraint);
    void fromFlatZinc(MiniZinc::Model& m);
    void* lookupVar(VarDecl* vd);
    void* lookupVar(std::string s);
    void* resolveVar(Expression* e) {
      if (e->isa<Id>()) {
	return lookupVar(e->cast<Id>()->_v.str());
      } else if (e->isa<ArrayAccess>()) {
	ArrayAccess* aa = e->cast<ArrayAccess>();
	void* var = resolveVar(aa->_v);
	int index = ((*aa->_idx)[0])->cast<IntLit>()->_v - 1;
	return resolveArrayAccess(var,index);
      } else if(e->isa<ArrayLit>()){
	return resolveArrayLit(e->cast<ArrayLit>());
      } else if (e->isa<IntLit>()) {
	return resolveIntLit(e->cast<IntLit>()->_v);
      } else if (e->isa<BoolLit>()) {
	return resolveBoolLit(e->cast<BoolLit>()->_v);
      } else if (e->isa<FloatLit>()) {
	return resolveFloatLit(e->cast<FloatLit>()->_v);
      } else if (e->isa<UnOp>()) {
	Expression* uo = e->cast<UnOp>()->_e0;
	if (uo->isa<IntLit>()) {
	  int v = getNumber<int, IntLit>(e);
	  return resolveIntLit(v);
	} else if (uo->isa<BoolLit>()) {
	  bool v = getNumber<bool, BoolLit>(e);
	  return resolveBoolLit(v);
	} else if (uo->isa<FloatLit>()) {
	  double v = getNumber<double, FloatLit>(e);
	  return resolveFloatLit(v);
	}
      }
      std::cerr << "Error " << e->_loc << std::endl
		<< "Variables should be identificators, array accesses, array literals, int literals, float literals, or bool literals." << std::endl 
		<< "Got : " << printEID(e->_eid) << std::endl
		<< "in : " << e;
      std::exit(-1);
    }
  protected:
    virtual void* addSolverVar(VarDecl*) = 0;
   

    typedef void (*poster) (SolverInterface&, const Call* call);
    
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
      else if(e->isa<TypeInst>()){
	TypeInst* ti = e->cast<TypeInst>();
	e = ti->_domain;
	if(e)
	  return getFloatBounds(e);
	else
	  throw -1;
      } 
      else {
	std::cerr << "getIntBounds : Expected BinOp or TypeInst, got this : " << printEID(e->_eid);
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
      else if(e->isa<TypeInst>()){
	TypeInst* ti = e->cast<TypeInst>();
	e = ti->_domain;
	if(e)
	  return getIntBounds(e);
	else
	  throw -1;
      } 
      else {
	std::cerr << "getIntBounds : Expected BinOp or TypeInst, got this : " << printEID(e->_eid);
	Printer::getInstance()->print(e);
	std::exit(0);
      }
    }
    poster lookupConstraint(std::string& s);
    std::map<VarDecl*, void*> variableMap;
    std::map<std::string, poster> constraintMap;
    /*enum ExpressionId {

      } _eid;*/
  public:
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
