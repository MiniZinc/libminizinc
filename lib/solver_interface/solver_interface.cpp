#include <minizinc/solver_interface/solver_interface.h>

namespace MiniZinc {
  void* SolverInterface::lookupVar(VarDecl* vd){
    std::map<VarDecl*,void*>::iterator it;
    it = variableMap.find(vd);
    if(it != variableMap.end()){
      return it->second;
    } else {
      std::cerr << "Error : couldn't find variable " << vd->_id.str()
		<< " in constraints map." << std::endl;
      throw -1;	    
    }
  }

  SolverInterface::poster SolverInterface::lookupConstraint(std::string& s){
    std::map<std::string, SolverInterface::poster>::iterator it 
      = constraintMap.find(s);
    if(it == constraintMap.end()){
      std::cerr << "Error : couldn't find constraint " << s
		<< " in constraints map." << std::endl;
      throw -1;
    } else return it->second;
  }

  void SolverInterface::addConstraintMapping(std::string& mzn_constraint,
					     SolverInterface::poster func){
    constraintMap[mzn_constraint] = func;
  }
};
