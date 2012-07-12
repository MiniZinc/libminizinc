#include <minizinc/ast.hh>
#include <minizinc/solver_interface/cplex_interface.h>
#include <ilcplex/ilocplex.h>
namespace MiniZinc {
  template<typename T, typename S>
  void p_lin(SolverInterface& si, const CtxVec<Expression*>& args,
	     CplexInterface::LIN_CON_TYPE lt,bool reif=false){
    IloModel* model = (IloModel*)(si.getModel());

    CtxVec<Expression*> *coeff = args[0]->cast<ArrayLit>()->_v;
    CtxVec<Expression*> *vars =  args[1]->cast<ArrayLit>()->_v;
    T res = args[2]->cast<S>()->_v;

    IloNum lb,ub;
    lb = -IloInfinity;
    ub = IloInfinity;
    switch(lt){
    case CplexInterface::LQ: ub = res;break;
    case CplexInterface::GQ: lb = res;break;
    case CplexInterface::EQ: ub = res; lb = res; break;
    }
    IloRange range(model->getEnv(),lb,ub);
    for(unsigned int i = 0; i < coeff->size(); i++){
      T co = ((*coeff)[i]->cast<S>())->_v;
      IloNumVar* v = (IloNumVar*)(si.resolveVar((*vars)[i]));
      range.setLinearCoef(*v,co);
    }if(reif){
      IloNumVar* varr = (IloNumVar*)(si.resolveVar(args[2]));
      model->add(IloConstraint(range == *varr));
    }
    model->add(range);
  }



  void p_array_element(SolverInterface& si, const CtxVec<Expression*>& vars){
    IloNumVar* varb = (IloNumVar*)(si.resolveVar(vars[0]));
    IloNumArray* varas = (IloNumArray*)(si.resolveVar(vars[1]));
    IloNumVar* varc = (IloNumVar*)(si.resolveVar(vars[2]));

    /*    IloConstraint constraint((*varas)[*varb] == *varc);
	  ((IloModel*)(si.getModel()))->add(constraint);*/
  }
  void p_array_bool_and(SolverInterface& si, const CtxVec<Expression*>& args){
    CtxVec<Expression*>& array = *(args[0]->cast<ArrayLit>()->_v);
    IloNumVar* var = (IloNumVar*)(si.resolveVar(args[1]));
    IloModel* model = (IloModel*)si.getModel();
    IloConstraint constraint;
    for(unsigned int i = 0; i < array.size(); i++){
      IloNumVar* array_element = (IloNumVar*)(si.resolveVar(array[i]));
      if(i==0){
	constraint = IloConstraint(*array_element == true);
      }else {
	constraint == constraint && IloConstraint(*array_element == true);
      }
    }
    
    model->add(IloConstraint(*var == constraint));
  }

  void p_bool_lin(SolverInterface& si, const CtxVec<Expression*>& args,
		  CplexInterface::LIN_CON_TYPE lt, bool reif = false){
    p_lin<bool,BoolLit>(si,args,lt, reif);
  }
  void p_bool_lin_le(SolverInterface& si, const CtxVec<Expression*>& args){
    p_bool_lin(si,args,CplexInterface::LQ);
  }
  
  void p_bool_lin_eq(SolverInterface& si, const CtxVec<Expression*>& args){
    p_bool_lin(si,args,CplexInterface::EQ);
  }
  void p_int_lin(SolverInterface& si, const CtxVec<Expression*>& args,
		 CplexInterface::LIN_CON_TYPE lt, bool reif = false){
    p_lin<int,IntLit>(si,args,lt, reif);
  }
  void p_int_lin_le(SolverInterface& si, const CtxVec<Expression*>& args, bool reif = false){
    p_int_lin(si,args,CplexInterface::LQ, reif);
  }
  void p_int_lin_le_noreif(SolverInterface& si, const CtxVec<Expression*>& args){
    p_int_lin_le(si,args);
  }
  void p_int_lin_le_reif(SolverInterface& si, const CtxVec<Expression*>& args){
    p_int_lin_le(si,args,true);
  }
  void p_int_lin_eq(SolverInterface& si, const CtxVec<Expression*>& args, bool reif = false){
    p_int_lin(si,args,CplexInterface::EQ, reif);
  }
  void p_int_lin_eq_noreif(SolverInterface& si, const CtxVec<Expression*>& args){
    p_int_lin_eq(si,args);
  }
  void p_int_lin_eq_reif(SolverInterface& si, const CtxVec<Expression*>& args){
    p_int_lin_eq(si,args);
  }
  void p_float_lin(SolverInterface& si, const CtxVec<Expression*>& args,
		   CplexInterface::LIN_CON_TYPE lt, bool reif = false){
    p_lin<float,FloatLit>(si,args,lt, reif);
  }
  void p_float_lin_le(SolverInterface& si, const CtxVec<Expression*>& args, bool reif = false){
    p_float_lin(si,args,CplexInterface::LQ, reif);
  }
  void p_float_lin_le_reif(SolverInterface& si, const CtxVec<Expression*>& args){
    p_float_lin_le(si,args,true);
  }
  void p_float_lin_le_noreif(SolverInterface& si, const CtxVec<Expression*>& args){
    p_float_lin_le(si,args);
  }
  void p_float_lin_eq(SolverInterface& si, const CtxVec<Expression*>& args, bool reif = false){
    p_float_lin(si,args,CplexInterface::EQ,reif);
  }
  void p_float_lin_eq_reif(SolverInterface& si, const CtxVec<Expression*>& args){
    p_float_lin_eq(si,args,true);
  }
  void p_float_lin_eq_noreif(SolverInterface& si, const CtxVec<Expression*>& args){
    p_float_lin_eq(si,args);
  }
  void p_eq(SolverInterface& si, const CtxVec<Expression*>& vars){
    IloNumVar* vara = (IloNumVar*)(si.resolveVar(vars[0]));
    IloNumVar* varb = (IloNumVar*)(si.resolveVar(vars[1]));
    IloModel* model = (IloModel*)si.getModel();
    IloConstraint constraint(*vara == *varb);
    model->add(constraint);
  }
  void p_eq_reif(SolverInterface& si, const CtxVec<Expression*>& vars){
    IloNumVar* vara = (IloNumVar*)(si.resolveVar(vars[0]));
    IloNumVar* varb = (IloNumVar*)(si.resolveVar(vars[1]));
    IloNumVar* varr = (IloNumVar*)(si.resolveVar(vars[2]));
    IloModel* model = (IloModel*)si.getModel();
    IloConstraint constraint(*varr == (*vara == *varb));
    model->add(constraint);
  }
  void p_int2float(SolverInterface& si, const CtxVec<Expression*>& vars){
    p_eq(si,vars);
  }
  void p_abs(SolverInterface& si, const CtxVec<Expression*>& vars){
    IloNumVar* vara = (IloNumVar*)(si.resolveVar(vars[0]));
    IloNumVar* varb = (IloNumVar*)(si.resolveVar(vars[1]));
    IloModel* model = (IloModel*)si.getModel();
    IloConstraint constraint(IloAbs(*vara) == *varb);
    model->add(constraint);
  }
  void p_le(SolverInterface& si, const CtxVec<Expression*>& vars){
    IloNumVar* vara = (IloNumVar*)(si.resolveVar(vars[0]));
    IloNumVar* varb = (IloNumVar*)(si.resolveVar(vars[1]));
    IloModel* model = (IloModel*)si.getModel();
    IloConstraint constraint(*vara <= *varb);
    model->add(constraint);
  }
  void p_le_reif(SolverInterface& si, const CtxVec<Expression*>& vars){
    IloNumVar* vara = (IloNumVar*)(si.resolveVar(vars[0]));
    IloNumVar* varb = (IloNumVar*)(si.resolveVar(vars[1]));
    IloNumVar* varr = (IloNumVar*)(si.resolveVar(vars[2]));
    IloModel* model = (IloModel*)si.getModel();
    IloConstraint constraint(*varr ==(*vara <= *varb));
    model->add(constraint);
  }
  void p_ne(SolverInterface& si, const CtxVec<Expression*>& vars){
    IloNumVar* vara = (IloNumVar*)(si.resolveVar(vars[0]));
    IloNumVar* varb = (IloNumVar*)(si.resolveVar(vars[1]));
    IloModel* model = (IloModel*)si.getModel();
    IloConstraint constraint(*vara != *varb);
    model->add(constraint);
  }
  void p_ne_reif(SolverInterface& si, const CtxVec<Expression*>& vars){
    IloNumVar* vara = (IloNumVar*)(si.resolveVar(vars[0]));
    IloNumVar* varb = (IloNumVar*)(si.resolveVar(vars[1]));
    IloNumVar* varr = (IloNumVar*)(si.resolveVar(vars[2]));
    IloModel* model = (IloModel*)si.getModel();
    IloConstraint constraint(*varr ==(*vara != *varb));
    model->add(constraint);
  }
  void p_plus(SolverInterface& si, const CtxVec<Expression*>& vars){
    IloNumVar* vara = (IloNumVar*)(si.resolveVar(vars[0]));
    IloNumVar* varb = (IloNumVar*)(si.resolveVar(vars[1]));
    IloNumVar* varc = (IloNumVar*)(si.resolveVar(vars[2]));
    IloModel* model = (IloModel*)si.getModel();
    IloConstraint constraint(*varc ==(*vara + *varb));
    model->add(constraint);
  }
  void p_times(SolverInterface& si, const CtxVec<Expression*>& vars){
    IloNumVar* vara = (IloNumVar*)(si.resolveVar(vars[0]));
    IloNumVar* varb = (IloNumVar*)(si.resolveVar(vars[1]));
    IloNumVar* varc = (IloNumVar*)(si.resolveVar(vars[2]));
    IloModel* model = (IloModel*)si.getModel();
    IloConstraint constraint(*varc ==(*vara * *varb));
    model->add(constraint);
  }
  void p_bool_and(SolverInterface& si, const CtxVec<Expression*>& vars){
    IloNumVar* vara = (IloNumVar*)(si.resolveVar(vars[0]));
    IloNumVar* varb = (IloNumVar*)(si.resolveVar(vars[1]));
    IloNumVar* varc = (IloNumVar*)(si.resolveVar(vars[2]));
    IloModel* model = (IloModel*)si.getModel();
    IloConstraint constraint(*varc == (IloConstraint(*vara==true) && IloConstraint(*varb==false)));
    model->add(constraint);
  }
  void p_bool_not(SolverInterface& si, const CtxVec<Expression*>& vars){
    IloNumVar* vara = (IloNumVar*)(si.resolveVar(vars[0]));
    IloNumVar* varb = (IloNumVar*)(si.resolveVar(vars[1]));
    IloModel* model = (IloModel*)si.getModel();
    IloConstraint constraint((*vara) ==  *varb);
    model->add(!constraint);
  }
  void p_bool_or(SolverInterface& si, const CtxVec<Expression*>& vars){
    IloNumVar* vara = (IloNumVar*)(si.resolveVar(vars[0]));
    IloNumVar* varb = (IloNumVar*)(si.resolveVar(vars[1]));
    IloNumVar* varc = (IloNumVar*)(si.resolveVar(vars[2]));
    IloModel* model = (IloModel*)si.getModel();
    IloConstraint constraint(*varc == (IloConstraint(*vara==true) || IloConstraint(*varb==false)));
    model->add(constraint);
  }
  void p_bool_xor(SolverInterface& si, const CtxVec<Expression*>& vars){
    IloNumVar* vara = (IloNumVar*)(si.resolveVar(vars[0]));
    IloNumVar* varb = (IloNumVar*)(si.resolveVar(vars[1]));
    IloNumVar* varc = (IloNumVar*)(si.resolveVar(vars[2]));
    IloModel* model = (IloModel*)si.getModel();
    IloConstraint constraint(*varc == (*vara !=  *varb));
    model->add(constraint);
  }
  
 
  CplexInterface::CplexInterface() {
    model = new IloModel(env);

    addConstraintMapping(std::string("int2float"), p_eq);
    addConstraintMapping(std::string("int_abs"), p_abs);
    addConstraintMapping(std::string("int_eq"), p_eq);
    addConstraintMapping(std::string("int_eq_reif"), p_eq_reif);
    addConstraintMapping(std::string("int_le"), p_le);
    addConstraintMapping(std::string("int_le_reif"), p_le_reif);
    addConstraintMapping(std::string("int_lin_eq"), p_int_lin_eq_noreif);//
    addConstraintMapping(std::string("int_lin_eq_reif"), p_int_lin_eq_reif);
    addConstraintMapping(std::string("int_lin_le"), p_int_lin_le_noreif);//
    addConstraintMapping(std::string("int_lin_le_reif"), p_int_lin_le_reif);
    addConstraintMapping(std::string("int_ne"), p_ne);
    addConstraintMapping(std::string("int_ne_reif"), p_ne_reif);
    addConstraintMapping(std::string("int_plus"), p_plus);
    addConstraintMapping(std::string("int_times"), p_times);
    addConstraintMapping(std::string("array_bool_and"), p_array_bool_and);
    // addConstraintMapping(std::string("array_bool_element"), p_array_bool_element);
    // addConstraintMapping(std::string("array_bool_or"), p_array_bool_or);
    // addConstraintMapping(std::string("array_bool_xor"), p_array_bool_xor);
    // addConstraintMapping(std::string("array_float_element"), p_array_float_element);
    // addConstraintMapping(std::string("array_int_element"), p_array_int_element);
    // addConstraintMapping(std::string("array_set_element"), p_array_set_element);
    // addConstraintMapping(std::string("array_var_bool_element"), p_array_var_bool_element);
    // addConstraintMapping(std::string("array_var_float_element"), p_array_var_float_element);
    // addConstraintMapping(std::string("array_var_int_element"), p_array_var_int_element);
    // addConstraintMapping(std::string("array_var_set_element"), p_array_var_set_element);
    addConstraintMapping(std::string("bool2int"), p_eq);
    addConstraintMapping(std::string("bool_and"), p_bool_and);
    //    addConstraintMapping(std::string("bool_clause"), p_bool_clause);
    addConstraintMapping(std::string("bool_eq"), p_eq);
    addConstraintMapping(std::string("bool_eq_reif"), p_eq_reif);
    addConstraintMapping(std::string("bool_le"), p_le);
    addConstraintMapping(std::string("bool_le_reif"), p_le_reif);
    addConstraintMapping(std::string("bool_lin_eq"), p_bool_lin_eq);
    addConstraintMapping(std::string("bool_lin_le"), p_bool_lin_le);
    addConstraintMapping(std::string("bool_not"), p_bool_not);
    addConstraintMapping(std::string("bool_or"), p_bool_or);
    addConstraintMapping(std::string("bool_xor"), p_bool_xor);
    addConstraintMapping(std::string("float_abs"), p_abs);
    addConstraintMapping(std::string("float_eq"), p_eq);
    addConstraintMapping(std::string("float_eq_reif"), p_eq_reif);
    addConstraintMapping(std::string("float_le"), p_le);
    addConstraintMapping(std::string("float_le_reif"), p_le_reif);
    addConstraintMapping(std::string("float_lin_eq"), p_float_lin_eq_noreif);//
    addConstraintMapping(std::string("float_lin_eq_reif"), p_float_lin_eq_reif);
    addConstraintMapping(std::string("float_lin_le"), p_float_lin_le_noreif);//
    addConstraintMapping(std::string("float_lin_le_reif"), p_float_lin_le_reif);

    addConstraintMapping(std::string("float_ne"), p_ne);
    addConstraintMapping(std::string("float_ne_reif"), p_ne_reif);
    addConstraintMapping(std::string("float_plus"), p_plus);

  }
  void* CplexInterface::resolveVar(Expression* e){
    if(e->isa<Id>()){
      return lookupVar(e->cast<Id>()->_v.str());
    }else if(e->isa<ArrayAccess>()){
      ArrayAccess* aa = e->cast<ArrayAccess>();
      IloNumVarArray *inva = 
	static_cast<IloNumVarArray*>(resolveVar(aa->_v));
      int index = (*aa->_idx)[0]->cast<IntLit>()->_v;
      return (void*)(&((*inva)[index]));
    }
    std::cerr << "Variables should be identificators or array accesses."
	      << std::endl;
    return NULL;
    
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
