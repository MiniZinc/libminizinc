#include <minizinc/ast.hh>
#include <minizinc/solver_interface/cplex_interface.h>
#include <ilcplex/ilocplex.h>
namespace MiniZinc {
  void p_int_lin_lq(SolverInterface& si, const ConstraintI& con){
    Call* c = con._e->cast<Call>();
    /*
      This call should be of this type :
      int_lin_lq([a,b,c],[x,y,z],r), and should be translated :
      (a*x+b*y+c*z<=r)
     */
    Expression* coeff =(*c->_args)[0];
    Expression* vars = (*c->_args)[1];
    Expression* res = (*c->_args)[2];
    
    ArrayLit* coeffArray = coeff->cast<ArrayLit>();//array : ._v
    ArrayLit* varsArray = vars->cast<ArrayLit>();
    IntLit* resLit = res->cast<IntLit>();//_v
    IloModel* model = (IloModel*)(si.getEnv());
    IloRange range(model->getEnv(),-IloInfinity,resLit->_v);
    for(int i = 0; i < coeffArray->_v->size(); i++){
      int co = (*coeffArray->_v)[i]->cast<IntLit>()->_v;
      IloNumVar* v = (IloNumVar*)
	si.lookupVar((*varsArray->_v)[i]
		     ->cast<Id>()->_v.str());
      range.setLinearCoef(*v,co);
    }
    model->add(range);
  }
  void CplexInterface::solve(SolveI* s){
    if(s->_st == SolveI::SolveType::ST_SAT){
      }else {
	IloObjective obj;
	if(s->_st == SolveI::SolveType::ST_MAX) obj = IloMaximize(env);
	else obj = IloMinimize(env);
	//Let's assume that the expression is a var
	IloNumVar* v = (IloNumVar*)lookupVar(s->_e->cast<Id>()->_v.str());
	obj.setLinearCoef(*v,1);
	model->add(obj);
      }

      IloCplex cplex(*model);
  
        // Optimize the problem and obtain solution.
        if ( !cplex.solve() ) {
	  std::cerr << "Failed to optimize LP" << std::endl;
           throw(-1);
        }
  
        IloNumArray vals(env);
	std::cout << "Solution status = " << cplex.getStatus() << std::endl;
	std::cout << "Solution value  = " << cplex.getObjValue() << std::endl;

	std::map<VarDecl*, void*>::iterator it;
	for(it = variableMap.begin(); it != variableMap.end(); it++){
	  std::cout <<  it->first->_id.str() << " = "
		    << cplex.getValue(*(IloNumVar*)(it->second)) << std::endl;
      
	}
    }

 CplexInterface::CplexInterface() {
   model = new IloModel(env);
    addConstraintMapping(std::string("int_lin_lq"),p_int_lin_lq);
  }
  CplexInterface::~CplexInterface(){
    model->end();
    delete model;
    env.end();
  }
  void* CplexInterface::getEnv(){
    return (void*)(model);
  }
  static  std::string typeToString(IloNumVar::Type type){
    switch(type){
    case ILOFLOAT: return "ilofloat";
    case ILOINT: return "iloint";
    case ILOBOOL: return "ilobool";
    }
    return "unknown";
  }
  void* CplexInterface::addSolverVar(VarDecl* vd){
      MiniZinc::TypeInst* ti = vd->_ti;
      IloNumVar::Type type;
      switch(ti->_type._bt){
      case Type::BT_INT:
	type = ILOINT;
	break;
      case Type::BT_BOOL:
	type = ILOBOOL;
	break;
      case Type::BT_FLOAT:
	type = ILOFLOAT;
	break;
      default:
	std::cerr << "This type of var is not handled by CPLEX."
		  << std::endl;
	std::exit(-1);
      }
      if(ti->isarray()){
	/*
	  IloNumVarArray* res
	  = new IloNumVarArray(env,IloNum(0),IloInfinity,type);
	  for(unsigned int i = 0; i < ti->_ranges->size(); i++){
	  //dimension i
	  Expression* range = (*(ti->_ranges))[i];
	  std::pair<double,double> rangebounds = getBounds(range);
	  }
	  Expression* domain = ti->_domain;
	  std::pair<int,int> bounds = getBounds(domain);
	  res->setBounds(bounds.first, bounds.second);
	  return (void*)res;*/
	return NULL;
      }
      else{
	Expression* domain = ti->_domain;


	IloNum lb, ub;
	if(domain){
	  std::pair<double,double> bounds;
	  if(type == ILOFLOAT){	 
	    bounds = getFloatBounds(domain);
	  } else if (type == ILOINT){
	    bounds = getIntBounds(domain);
	  }
	  lb = IloNum(bounds.first);
	  ub = IloNum(bounds.second);
	}
	else {
	  lb = -IloInfinity;
	  ub = IloInfinity;	  
	}
	IloNumVar* res = new IloNumVar(env, lb, ub, type);
	//std::cout << "Added var with bounds ["<<lb<<","<<ub<<"]"
	//  << " of type " << typeToString(type) << std::endl;

	
	return (void*)res;
      }
  }

};
