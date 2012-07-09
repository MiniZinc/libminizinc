#ifndef __SOLVER_INTERFACE_H
#define __SOLVER_INTERFACE_H

#include <map>
#include <minizinc/ast.hh>
#include <minizinc/parser.hh>
#include <minizinc/model.hh>
namespace MiniZinc{
enum SolverType {
	SolverInt, SolverFloat, SolverBool
};

class SolverDomain {

};

template <class T> class SolverRange : public SolverDomain {
	private:
	T lb;
	T ub;
	public:
	SolverRange(T l, T u) : lb(l), ub(u) {}
	T getLowerBound() {return lb;}
	T getUpperBound() {return ub;}
};

 class SolverModel {
 };

class SolverInterface {
protected:
	SolverInterface();
	virtual ~SolverInterface() = 0;
public:
	virtual void addVar(VarDecl* vd, SolverType& type, SolverDomain& domain){
	  void* solverVar = addSolverVar(vd, type, domain);
	  variableMap.insert(std::pair<VarDecl*, void*>(vd,solverVar));
	}
	virtual void postConstraint(SolverModel& model, ConstraintI& constraint){
	  std::string con_id = constraint._e._id;
	  std::map<std::string, poster>::iterator it = constraintMap.find(con_id);
	  if(it == constraintMap.end()){
	    std::cerr << "Error : couldn't find constraint " << con_id 
		      << " in constraints map." << std::endl;
	    throw -1;
	  }
	  it->second(model,constraint);
	}
	virtual void solve();
	typedef void (*poster) (SolverModel&, const ConstraintI&);
private:
	virtual void* solverVar(VarDecl*, SolverType& st, SolverDomain& sd) = 0;
	std::map<VarDecl*, void*> variableMap;
	std::map<std::string, poster> constraintMap;
	
};
}
#endif
