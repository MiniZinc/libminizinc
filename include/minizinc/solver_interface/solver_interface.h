#ifndef __SOLVER_INTERFACE_H
#define __SOLVER_INTERFACE_H

#include <map>
#include <minizinc/ast.hh>
#include <minizinc/parser.hh>
#include <minizinc/model.hh>
namespace MiniZinc{


  class SolverInterface {
  public:
    SolverInterface(){}
    virtual ~SolverInterface(){}
    void addVar(VarDecl* vd){
      void* solverVar = addSolverVar(vd);
      variableMap.insert(std::pair<VarDecl*, void*>(vd,solverVar));
    }
    void postConstraint(ConstraintI& constraint){
      Call* c = constraint._e->cast<Call>();
      std::string con_id = c->_id.str();
      std::map<std::string, poster>::iterator it = constraintMap.find(con_id);
      if(it == constraintMap.end()){
	std::cerr << "Error : couldn't find constraint " << con_id 
		  << " in constraints map." << std::endl;
	throw -1;
      }
      it->second(*this,constraint);
    }
    void fromFlatZinc(MiniZinc::Model& m){
      for(int i = 0; i < m._items.size(); i++){
	Item* item = m._items[i];
	if(item->isa<VarDeclI>()){
	  addVar(item->cast<VarDeclI>()->_e);
	} else if (item->isa<ConstraintI>()){
	  postConstraint(*item->cast<ConstraintI>());
	}
      }
    }
	
	
  protected:
    typedef void (*poster) (SolverInterface&, const ConstraintI&);
    virtual void* addSolverVar(VarDecl*) = 0;
    virtual void solve() = 0;
    void addConstraintMapping(std::string& mzn_constraint,
			      poster func);
    void* lookupVar(VarDecl* vd);
    poster lookupConstraint(std::string& s);
  private:
    std::map<VarDecl*, void*> variableMap;
    std::map<std::string, poster> constraintMap;
  };
};
#endif
