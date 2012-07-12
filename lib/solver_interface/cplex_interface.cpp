#include <minizinc/ast.hh>
#include <minizinc/solver_interface/cplex_interface.h>
#include <ilcplex/ilocplex.h>
namespace MiniZinc {
  template<typename T, typename S>
  void p_lin(SolverInterface& si, const Call* c,
	     CplexInterface::LIN_CON_TYPE lt){
    IloModel* model = (IloModel*)(si.getModel());

    CtxVec<Expression*> *coeff = ((*c->_args)[0]->cast<ArrayLit>()->_v);
    CtxVec<Expression*> *vars =  ((*c->_args)[1]->cast<ArrayLit>()->_v);
    T res = (*c->_args)[2]->cast<S>()->_v;

    IloNum lb,ub;
    lb = -IloInfinity;
    ub = IloInfinity;
    switch(lt){
    case CplexInterface::LQ: ub = res;break;
    case CplexInterface::GQ: lb = res;break;
    }
    IloRange range(model->getEnv(),lb,ub);
    for(unsigned int i = 0; i < coeff->size(); i++){
      T co = ((*coeff)[i]->cast<S>())->_v;

      IloNumVar* v = (IloNumVar*)(si.resolveVar(si,(*vars)[i]));
      range.setLinearCoef(*v,co);
    }
    
    model->add(range);
  }

  void p_int_lin(SolverInterface& si, const Call* c,
		 CplexInterface::LIN_CON_TYPE lt){
    p_lin<int,IntLit>(si,c,lt);
  }
  void p_int_lin_lq(SolverInterface& si, const Call* c){
    p_int_lin(si,c,CplexInterface::LQ);
  }
  void p_float_lin(SolverInterface& si, const Call* c,
		   CplexInterface::LIN_CON_TYPE lt){
    p_lin<float,FloatLit>(si,c,lt);
  }
  void p_float_lin_lq(SolverInterface& si, const Call* c){
    p_float_lin(si,c,CplexInterface::LQ);
  }
 
  CplexInterface::CplexInterface() {
    model = new IloModel(env);
    addConstraintMapping(std::string("float_lin_le"),p_float_lin_lq);
    addConstraintMapping(std::string("int_lin_lq"),p_int_lin_lq);
  }
  void* CplexInterface::resolveVar(SolverInterface& si, Expression* e){
    if(e->isa<Id>()){
      return si.lookupVar(e->cast<Id>()->_v.str());
    }else if(e->isa<ArrayAccess>()){
      ArrayAccess* aa = e->cast<ArrayAccess>();
      IloNumVarArray *inva = 
	static_cast<IloNumVarArray*>(resolveVar(si,aa->_v));
      int index = (*aa->_idx)[0]->cast<IntLit>()->_v;
      return (void*)(&((*inva)[index]));
    }else{
      std::cerr << "Variables should be identificators or array accesses."
		<< std::endl;
      return NULL;
    }
  }
 
  void CplexInterface::solve(SolveI* s){
    if(s->_st != SolveI::SolveType::ST_SAT){
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
      std::cout << "Failed to optimize LP" << std::endl;
      return;
    }

    std::cout << "Solution status = " << cplex.getStatus() << std::endl;
    std::cout << "Solution value  = " << cplex.getObjValue() << std::endl;
    std::cout << showVariables(cplex);
    
  }
  std::string CplexInterface::showVariable(IloCplex& cplex, IloNumVar& v){
    std::ostringstream oss;
    try{
      IloNum num = cplex.getValue(v);
      oss << num;
    } catch(IloAlgorithm::NotExtractedException& e) {
      oss << "_";
      // TODO : show possible values ?
      /*IloNumArray posval(env);
      v.getPossibleValues(posval);
      int size = posval.getSize();
      oss << "{" ;
      for(int j = 0; j < size; j++){
	oss << posval[j];
	if(j != size - 1) oss << ", ";
      }
      oss << "}";*/
    }
    return oss.str();
  }
  std::string CplexInterface::showVariables(IloCplex& cplex){
    std::ostringstream oss;
    std::map<VarDecl*, void*>::iterator it;
    for(it = variableMap.begin(); it != variableMap.end(); it++){
      oss <<  it->first->_id.str() << " = ";
      if(it->first->_ti->isarray()){
	IloNumVarArray* varray = static_cast<IloNumVarArray*>(it->second);
	oss << "array[";
	int size = varray->getSize();
	for(int i = 0; i < size; i++){
	  IloNumVar& v = (*varray)[i];
	  oss << showVariable(cplex,v);
	  if(i != size -1) oss << ", ";
	}
	oss << "]";
      } else {
	oss << showVariable(cplex,*(IloNumVar*)(it->second));
      }
      oss << std::endl;     
    }
    return oss.str();
  }

  CplexInterface::~CplexInterface(){
    model->end();
    delete model;
    env.end();
  }
  void* CplexInterface::getModel(){
    return (void*)(model);
  }
  // static  std::string typeToString(IloNumVar::Type type){
  //   switch(type){
  //   case ILOFLOAT: return "ilofloat";
  //   case ILOINT: return "iloint";
  //   case ILOBOOL: return "ilobool";
  //   }
  //   return "unknown";
  // }
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
    if(ti->isarray()){
      assert(ti->_ranges->size() == 1);
      Expression* range = (*(ti->_ranges))[0];
      std::pair<int,int> rangebounds = getIntBounds(range);
      int rangesize = rangebounds.second - rangebounds.first;
      IloNumVarArray* res = new IloNumVarArray(env,rangesize+1,lb,ub,type);
      return (void*)res;
    }
    else{
      IloNumVar* res = new IloNumVar(env, lb, ub, type);
      return (void*)res;
    }
  }

};
