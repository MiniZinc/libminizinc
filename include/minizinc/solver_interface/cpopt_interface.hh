#ifndef __CPOPT_INTERFACE_H
#define __CPOPT_INTERFACE_H

#include "solver_interface.hh"
#include <ilcp/cp.h>
#include <string>

namespace MiniZinc {
  class CpOptInterface : public SolverInterface {
  private:
    IloEnv env;
    IloModel* model;
  protected:

    void* addSolverVar(VarDecl* vd);
  public:
    CpOptInterface();
    virtual ~CpOptInterface();

    void* getModel();
    void solve(SolveI* s);
    std::string showVariables(IloCP& cplex);
    std::string showVariable(IloCP& cplex, IloNumVar& v);
    enum LIN_CON_TYPE {LQ,EQ,GQ};
    template<typename S, typename T> void initArray(IloNumVarArray& res, CtxVec<Expression*>& ar);
   
    void* resolveArrayAccess(void* array, int index){
      IloNumVarArray *inva = static_cast<IloNumVarArray*>(array);    
      return (void*) (&((*inva)[index]));
    }
    template<typename T> void* resolveLit(T v, IloNumVar::Type type){
      IloExpr* var = new IloExpr(model->getEnv(), v);
      return (void*) var;
    }
    void* resolveIntLit(int v){
      return resolveLit<int>(v,ILOINT);
    }
    void* resolveBoolLit(bool v){
      return resolveLit<bool>(v,ILOBOOL);
    }
    void* resolveFloatLit(double v){
      return resolveLit<double>(v,ILOFLOAT);
    }
    void* resolveArrayLit(ArrayLit* al){
      unsigned int size = (*al->_dims)[0].second;
      if(size == 0){
	std::cerr << "ArrayLit of size 0 in resolveArrayLit" << std::endl;
      }

      Expression* e = (*al->_v)[0];
      if(e->isa<Id>()){
	IloIntVarArray* res = new IloIntVarArray(model->getEnv(), size);
	for(unsigned int i = 0; i < size; i++){
	  (*res)[i] = (*((IloIntVar*)resolveVar((*al->_v)[i])));
	}
	return (void*) res;
      } else {
	IloIntArray* res = new IloIntArray(model->getEnv(), size);
	for(unsigned int i = 0; i < size; i++){
	  (*res)[i] = (*((IloInt*)resolveVar((*al->_v)[i])));
	}
	return (void*) res;
      }
    }
    
  
  };
};
#endif
