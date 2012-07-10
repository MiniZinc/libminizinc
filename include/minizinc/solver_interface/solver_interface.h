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
	} else if (item->isa<SolveI>()){	  
	  solve(item->cast<SolveI>());
	} else {
	  
	  //  std::cerr << "This type of item should not appear in a FlatZinc file"<<item->_iid << std::endl;
	  //std::exit(-1);
	}
      }
    }
	
    virtual void* getEnv()=0;
    void* lookupVar(VarDecl* vd);
    void* lookupVar(std::string s);	
  protected:
    typedef void (*poster) (SolverInterface&, const ConstraintI&);
    virtual void* addSolverVar(VarDecl*) = 0;
    virtual void solve(SolveI* s) = 0;
    void addConstraintMapping(std::string mzn_constraint,
			      poster func);
    static std::pair<double,double> getFloatBounds(Expression* e){
      BinOp* bo = e->cast<BinOp>();
      double b, u;
      b = bo->_e0->cast<FloatLit>()->_v;
      u = bo->_e1->cast<FloatLit>()->_v;
      return std::pair<double,double>(b,u);
    }
    static std::pair<double,double> getIntBounds(Expression* e){
      BinOp* bo = e->cast<BinOp>();
      int b, u;
      b = bo->_e0->cast<IntLit>()->_v;
      u = bo->_e1->cast<IntLit>()->_v;
      return std::pair<int,int>(b,u);
    }
    poster lookupConstraint(std::string& s);
    std::map<VarDecl*, void*> variableMap;
    std::map<std::string, poster> constraintMap;
  };
};
#endif
