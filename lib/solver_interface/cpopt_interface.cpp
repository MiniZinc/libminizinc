#include <minizinc/ast.hh>
#include <minizinc/solver_interface/cpopt_interface.hh>
#include <ilcp/cp.h>
#include <minizinc/printer.hh>

namespace MiniZinc {
  
  namespace CpOptConstraints{
    template<typename T, typename S>
    void p_lin(SolverInterface& si, const Call* call,
	       CpOptInterface::LIN_CON_TYPE lt, bool reif = false) {
      IloModel* model = (IloModel*) (si.getModel());
      CtxVec<Expression*>& args = *(call->_args);
      CtxVec<Expression*> *coeff = args[0]->cast<ArrayLit>()->_v;
      CtxVec<Expression*> *vars = args[1]->cast<ArrayLit>()->_v;
      T res = getNumber<T, S>(args[2]);

      IloNum lb, ub;
      lb = -IloInfinity;
      ub = IloInfinity;
      switch (lt) {
      case CpOptInterface::LQ:
	ub = res;
	break;
      case CpOptInterface::GQ:
	lb = res;
	break;
      case CpOptInterface::EQ:
	ub = res;
	lb = res;
	break;
      }
      IloRange range(model->getEnv(), lb, ub);

      for (unsigned int i = 0; i < coeff->size(); i++) {
	T co = getNumber<T, S>((*coeff)[i]);
	IloNumVar* v = (IloNumVar*) (si.resolveVar((*vars)[i]));
	range.setLinearCoef(*v, co);
      }
      if (reif) {
	IloExpr* varr = (IloExpr*) (si.resolveVar(args[3]));
	model->add(IloConstraint(range == *varr));
      } else model->add(range);
    }

    void p_array_bool_op(SolverInterface& si, const Call* call, IloConstraint (*op)(IloConstraint,IloConstraint)) {
      CtxVec<Expression*>& args = *(call->_args);
      IloExpr* var = (IloExpr*) (si.resolveVar(args[1]));
      IloModel* model = (IloModel*) si.getModel();
      IloConstraint constraint;
      if (args[0]->isa<ArrayLit>()) {
	CtxVec<Expression*>& array = *(args[0]->cast<ArrayLit>()->_v);
	for (unsigned int i = 0; i < array.size(); i++) {
	  IloExpr* array_element = (IloExpr*) (si.resolveVar(array[i]));
	  if (i == 0) {
	    constraint = IloConstraint(*array_element == true);
	  } else {
	    constraint = op(constraint,IloConstraint(*array_element == true));
	  }
	}
      } else if (args[0]->isa<Id>()) {
	IloExprArray* array = (IloExprArray*) si.resolveVar(args[0]);

	for (unsigned int i = 0; i < array->getSize(); i++) {
	  IloExpr* array_element = (IloExpr*) (&(*array)[i]);
	  if (i == 0) {
	    constraint = IloConstraint(*array_element == true);
	  } else {
	    constraint = op(constraint,IloConstraint(*array_element == true));
	  }
	}
      }

      model->add(IloConstraint(*var == constraint));
    }

    void p_array_bool_and(SolverInterface& si, const Call* call){
      p_array_bool_op(si,call,[](IloConstraint c1, IloConstraint c2) {
	  return IloConstraint(c1 && c2);
	});
    }
    void p_array_bool_or(SolverInterface& si, const Call* call){
      p_array_bool_op(si,call,[](IloConstraint c1, IloConstraint c2) {
	  return IloConstraint(c1 || c2);
	});
    }
   

    void p_bool_lin(SolverInterface& si, const Call* call,
		    CpOptInterface::LIN_CON_TYPE lt, bool reif = false) {
      p_lin<bool, BoolLit>(si, call, lt, reif);
    }
    void p_bool_lin_le(SolverInterface& si, const Call* call) {
      p_bool_lin(si, call, CpOptInterface::LQ);
    }

    void p_bool_lin_eq(SolverInterface& si, const Call* call) {
      p_bool_lin(si, call, CpOptInterface::EQ);
    }
    void p_int_lin(SolverInterface& si, const Call* call,
		   CpOptInterface::LIN_CON_TYPE lt, bool reif = false) {
      p_lin<int, IntLit>(si, call, lt, reif);
    }
    void p_int_lin_le(SolverInterface& si, const Call* call,
		      bool reif = false) {
      p_int_lin(si, call, CpOptInterface::LQ, reif);
    }
    void p_int_lin_le_noreif(SolverInterface& si, const Call* call) {
      p_int_lin_le(si, call);
    }
    void p_int_lin_le_reif(SolverInterface& si, const Call* call) {
      p_int_lin_le(si, call, true);
    }
    void p_int_lin_eq(SolverInterface& si, const Call* call,
		      bool reif = false) {
      p_int_lin(si, call, CpOptInterface::EQ, reif);
    }
    void p_int_lin_eq_noreif(SolverInterface& si, const Call* call) {
      p_int_lin_eq(si, call);
    }
    void p_int_lin_eq_reif(SolverInterface& si, const Call* call) {
      p_int_lin_eq(si, call, true);
    }
    void p_eq(SolverInterface& si, const Call* call) {
      CtxVec<Expression*>& args = *(call->_args);
      IloExpr* vara = (IloExpr*) (si.resolveVar(args[0]));
      IloExpr* varb = (IloExpr*) (si.resolveVar(args[1]));
      IloModel* model = (IloModel*) si.getModel();
      Expression* ann = call->_ann;
      while(ann){
	ann = ann->cast<Annotation>()->_e;
	if(ann->isa<Call>()){
	  Call* c = ann->cast<Call>();
	  if(c->_id.str() == "defines_var"){
	    Expression* arg = (*(c->_args))[0];
	    if(arg->isa<Id>()){
	      IloExpr* var_defined = (IloExpr*)(si.resolveVar(arg));
	      if(var_defined == vara){
		si.addVar(arg->cast<Id>()->_decl,(void*)varb);
		return;
	      } else if (var_defined == varb) {
		si.addVar(arg->cast<Id>()->_decl,(void*)vara);
		return;
	      }
	    }
	  }
	}
	ann = ann->_ann;
      }
      IloConstraint constraint(*vara == *varb);

      model->add(constraint);
    }
    void p_eq_reif(SolverInterface& si, const Call* call) {
      CtxVec<Expression*>& args = *(call->_args);
      IloExpr* vara = (IloExpr*) (si.resolveVar(args[0]));
      IloExpr* varb = (IloExpr*) (si.resolveVar(args[1]));
      IloExpr* varr = (IloExpr*) (si.resolveVar(args[2]));
      IloModel* model = (IloModel*) si.getModel();
      IloConstraint constraint(*varr == (*vara == *varb));
      model->add(constraint);
    }
    void p_abs(SolverInterface& si, const Call* call) {
      CtxVec<Expression*>& args = *(call->_args);
      IloExpr* vara = (IloExpr*) (si.resolveVar(args[0]));
      IloExpr* varb = (IloExpr*) (si.resolveVar(args[1]));
      IloModel* model = (IloModel*) si.getModel();
      IloConstraint constraint(IloAbs(*vara) == *varb);
      model->add(constraint);
    }
    void p_le(SolverInterface& si, const Call* call) {
      CtxVec<Expression*>& args = *(call->_args);
      IloExpr* vara = (IloExpr*) (si.resolveVar(args[0]));
      IloExpr* varb = (IloExpr*) (si.resolveVar(args[1]));
      IloModel* model = (IloModel*) si.getModel();
      IloConstraint constraint(*vara <= *varb);
      model->add(constraint);
    }
    void p_le_reif(SolverInterface& si, const Call* call) {
      CtxVec<Expression*>& args = *(call->_args);
      IloExpr* vara = (IloExpr*) (si.resolveVar(args[0]));
      IloExpr* varb = (IloExpr*) (si.resolveVar(args[1]));
      IloExpr* varr = (IloExpr*) (si.resolveVar(args[2]));
      IloModel* model = (IloModel*) si.getModel();
      IloConstraint constraint(*varr == (*vara <= *varb));
      model->add(constraint);
    }
    void p_ne(SolverInterface& si, const Call* call) {
      CtxVec<Expression*>& args = *(call->_args);
      IloExpr* vara = (IloExpr*) (si.resolveVar(args[0]));
      IloExpr* varb = (IloExpr*) (si.resolveVar(args[1]));
      IloModel* model = (IloModel*) si.getModel();
      IloConstraint constraint(*vara != *varb);
      model->add(constraint);
    }
    void p_ne_reif(SolverInterface& si, const Call* call) {
      CtxVec<Expression*>& args = *(call->_args);
      IloExpr* vara = (IloExpr*) (si.resolveVar(args[0]));
      IloExpr* varb = (IloExpr*) (si.resolveVar(args[1]));
      IloExpr* varr = (IloExpr*) (si.resolveVar(args[2]));
      IloModel* model = (IloModel*) si.getModel();
      IloConstraint constraint(*varr == (*vara != *varb));
      model->add(constraint);
    }
    void p_plus(SolverInterface& si, const Call* call) {
      CtxVec<Expression*>& args = *(call->_args);
      IloExpr* vara = (IloExpr*) (si.resolveVar(args[0]));
      IloExpr* varb = (IloExpr*) (si.resolveVar(args[1]));
      IloExpr* varc = (IloExpr*) (si.resolveVar(args[2]));
      IloModel* model = (IloModel*) si.getModel();
      IloConstraint constraint(*varc == (*vara + *varb));
      model->add(constraint);

    }
    void p_times_le(SolverInterface& si, const Call* call) {
      CtxVec<Expression*>& args = *(call->_args);
      IloExpr* vara = (IloExpr*) (si.resolveVar(args[0]));
      IloExpr* varb = (IloExpr*) (si.resolveVar(args[1]));
      IloExpr* varc = (IloExpr*) (si.resolveVar(args[2]));
      IloModel* model = (IloModel*) si.getModel();
      IloConstraint constraint((*vara * *varb) <= *varc);
      model->add(constraint);
    }
    void p_bool_and(SolverInterface& si, const Call* call) {
      CtxVec<Expression*>& args = *(call->_args);
      IloExpr* vara = (IloExpr*) (si.resolveVar(args[0]));
      IloExpr* varb = (IloExpr*) (si.resolveVar(args[1]));
      IloExpr* varc = (IloExpr*) (si.resolveVar(args[2]));
      IloModel* model = (IloModel*) si.getModel();
      IloConstraint constraint(
			       *varc
			       == (IloConstraint(*vara == true)
				   && IloConstraint(*varb == true)));
      model->add(constraint);
    }
    void p_bool_not(SolverInterface& si, const Call* call) {
      CtxVec<Expression*>& args = *(call->_args);
      IloExpr* vara = (IloExpr*) (si.resolveVar(args[0]));
      IloExpr* varb = (IloExpr*) (si.resolveVar(args[1]));
      IloModel* model = (IloModel*) si.getModel();
      IloConstraint constraint((*vara) == *varb);
      model->add(!constraint);
    }
    void p_bool_or(SolverInterface& si, const Call* call) {
      CtxVec<Expression*>& args = *(call->_args);
      IloExpr* vara = (IloExpr*) (si.resolveVar(args[0]));
      IloExpr* varb = (IloExpr*) (si.resolveVar(args[1]));
      IloExpr* varc = (IloExpr*) (si.resolveVar(args[2]));
      IloModel* model = (IloModel*) si.getModel();
      IloConstraint constraint(
			       *varc
			       == (IloConstraint(*vara == true)
				   || IloConstraint(*varb == true)));
      model->add(constraint);
    }

    void p_bool_xor(SolverInterface& si, const Call* call) {
      CtxVec<Expression*>& args = *(call->_args);
      IloExpr* vara = (IloExpr*) (si.resolveVar(args[0]));
      IloExpr* varb = (IloExpr*) (si.resolveVar(args[1]));
      IloExpr* varc = (IloExpr*) (si.resolveVar(args[2]));
      IloModel* model = (IloModel*) si.getModel();
      IloConstraint constraint(*varc == (*vara != *varb));
      model->add(constraint);
    }
    void p_lt(SolverInterface& si, const Call* call){
      CtxVec<Expression*>& args = *(call->_args);
      IloExpr* vara = (IloExpr*) (si.resolveVar(args[0]));
      IloExpr* varb = (IloExpr*) (si.resolveVar(args[1]));
      IloModel* model = (IloModel*)si.getModel();
      model->add(IloConstraint(*vara < *varb));
    }
    void p_lt_reif(SolverInterface& si, const Call* call){
      CtxVec<Expression*>& args = *(call->_args);
      IloExpr* vara = (IloExpr*) (si.resolveVar(args[0]));
      IloExpr* varb = (IloExpr*) (si.resolveVar(args[1]));
      IloExpr* varc = (IloExpr*) (si.resolveVar(args[2]));
      IloModel* model = (IloModel*)si.getModel();
      model->add(IloConstraint(*varc == (*vara < *varb)));
    }
    void p_div(SolverInterface& si, const Call* call){
      CtxVec<Expression*>& args = *(call->_args);
      IloIntExpr* vara = (IloIntExpr*) (si.resolveVar(args[0]));
      IloIntExpr* varb = (IloIntExpr*) (si.resolveVar(args[1]));
      IloIntExpr* varc = (IloIntExpr*) (si.resolveVar(args[2]));
      IloModel* model = (IloModel*)si.getModel();
      model->add(IloConstraint(*varc == IloDiv(*vara, *varb)));
    }
    void p_min(SolverInterface& si, const Call* call){
      CtxVec<Expression*>& args = *(call->_args);
      IloExpr* vara = (IloExpr*) (si.resolveVar(args[0]));
      IloExpr* varb = (IloExpr*) (si.resolveVar(args[1]));
      IloExpr* varc = (IloExpr*) (si.resolveVar(args[2]));
      IloModel* model = (IloModel*)si.getModel();
      model->add(IloConstraint(*varc == IloMin(*vara, *varb)));
    }
    void p_max(SolverInterface& si, const Call* call){
      CtxVec<Expression*>& args = *(call->_args);
      IloExpr* vara = (IloExpr*) (si.resolveVar(args[0]));
      IloExpr* varb = (IloExpr*) (si.resolveVar(args[1]));
      IloExpr* varc = (IloExpr*) (si.resolveVar(args[2]));
      IloModel* model = (IloModel*)si.getModel();
      model->add(IloConstraint(*varc == IloMax(*vara, *varb)));
    }
    void p_mod(SolverInterface& si, const Call* call){
      CtxVec<Expression*>& args = *(call->_args);
      IloIntExpr* vara = (IloIntExpr*) (si.resolveVar(args[0]));
      IloIntExpr* varb = (IloIntExpr*) (si.resolveVar(args[1]));
      IloIntExpr* varc = (IloIntExpr*) (si.resolveVar(args[2]));
      IloModel* model = (IloModel*)si.getModel();
      model->add(IloConstraint(*varc == (*vara % *varb)));
    }
    void p_times(SolverInterface& si, const Call* call){
      CtxVec<Expression*>& args = *(call->_args);
      IloIntExpr* vara = (IloIntExpr*) (si.resolveVar(args[0]));
      IloIntExpr* varb = (IloIntExpr*) (si.resolveVar(args[1]));
      IloIntExpr* varc = (IloIntExpr*) (si.resolveVar(args[2]));
      IloModel* model = (IloModel*)si.getModel();
      model->add(IloConstraint(*varc == (*vara * *varb)));
    }
    void p_array_element(SolverInterface& si, const Call* call){
      CtxVec<Expression*>& args = *(call->_args);
      IloIntExpr* var_index = (IloIntExpr*) (si.resolveVar(args[0]));
      IloNumExprArray* var_array = (IloNumExprArray*) (si.resolveVar(args[1]));
      IloNumExpr* var_res = (IloNumExpr*) (si.resolveVar(args[2]));
      std::cout << *var_index << " -- " << *var_array << " -- " << *var_res << std::endl;

      IloModel* model = (IloModel*)si.getModel();
      //creating an array with dummy value
      IloNumExprArray* ar = new IloNumExprArray(model->getEnv());
      ar->add(IloNumExpr(model->getEnv(),0));
      ar->add(*var_array);
      model->add(IloConstraint(*var_res ==(*ar)[*var_index]));
    }
    void p_array_var_element(SolverInterface& si, const Call* call){
      CtxVec<Expression*>& args = *(call->_args);
      IloIntExpr* var_index = (IloIntExpr*) (si.resolveVar(args[0]));
      IloIntVarArray* var_array = (IloIntVarArray*) (si.resolveVar(args[1]));
      IloNumExpr* var_res = (IloNumExpr*) (si.resolveVar(args[2]));
      IloModel* model = (IloModel*)si.getModel();
      IloIntVarArray* ar = new IloIntVarArray(model->getEnv());
      ar->add(IloIntVar(model->getEnv(),0,0));
      ar->add(*var_array);
      model->add(IloConstraint((*ar)[*var_index] == *var_res));
    }
    void p_bool_clause(SolverInterface& si, const Call* call){
      CtxVec<Expression*>& args = *(call->_args);
      IloExprArray* vara = (IloExprArray*) (si.resolveVar(args[0]));
      IloExprArray* varb = (IloExprArray*) (si.resolveVar(args[1]));
      IloModel* model = (IloModel*)si.getModel();
      IloOr constraint(model->getEnv());
      unsigned int sizeA = vara->getSize();
      unsigned int sizeB = varb->getSize();
      for(unsigned int i = 0; i < sizeA; i++){
	IloExpr e = (*vara)[i];
	constraint.add(e == IloTrue);
      }
      for(unsigned int i = 0; i < sizeB; i++){
	IloExpr e = (*varb)[i];
	constraint.add(e == IloFalse);
      }
      model->add(constraint);
    }
    

  }
  CpOptInterface::CpOptInterface() {
    model = new IloModel(env);
    addConstraintMapping(std::string("int_abs"), CpOptConstraints::p_abs);
    addConstraintMapping(std::string("int_eq"), CpOptConstraints::p_eq);
    addConstraintMapping(std::string("int_eq_reif"), CpOptConstraints::p_eq_reif);
    addConstraintMapping(std::string("int_le"), CpOptConstraints::p_le);
    addConstraintMapping(std::string("int_le_reif"), CpOptConstraints::p_le_reif);
    addConstraintMapping(std::string("int_lin_eq"), CpOptConstraints::p_int_lin_eq_noreif); //
    addConstraintMapping(std::string("int_lin_eq_reif"), CpOptConstraints::p_int_lin_eq_reif);
    addConstraintMapping(std::string("int_lin_le"), CpOptConstraints::p_int_lin_le_noreif); //
    addConstraintMapping(std::string("int_lin_le_reif"), CpOptConstraints::p_int_lin_le_reif);

    addConstraintMapping(std::string("int_ne"), CpOptConstraints::p_ne);
    addConstraintMapping(std::string("int_ne_reif"), CpOptConstraints::p_ne_reif);
    addConstraintMapping(std::string("int_plus"), CpOptConstraints::p_plus);
    addConstraintMapping(std::string("int_times_le"), CpOptConstraints::p_times_le);
    addConstraintMapping(std::string("array_bool_and"), CpOptConstraints::p_array_bool_and);
    addConstraintMapping(std::string("array_bool_or"), CpOptConstraints::p_array_bool_or);
   

    addConstraintMapping(std::string("bool2int"), CpOptConstraints::p_eq);
    addConstraintMapping(std::string("bool_and"), CpOptConstraints::p_bool_and);
    addConstraintMapping(std::string("bool_eq"), CpOptConstraints::p_eq);
    addConstraintMapping(std::string("bool_eq_reif"), CpOptConstraints::p_eq_reif);
    addConstraintMapping(std::string("bool_le"), CpOptConstraints::p_le);
    addConstraintMapping(std::string("bool_le_reif"), CpOptConstraints::p_le_reif);
    addConstraintMapping(std::string("bool_lin_eq"), CpOptConstraints::p_bool_lin_eq);
    addConstraintMapping(std::string("bool_lin_le"), CpOptConstraints::p_bool_lin_le);
    addConstraintMapping(std::string("bool_not"), CpOptConstraints::p_bool_not);
    addConstraintMapping(std::string("bool_or"), CpOptConstraints::p_bool_or);
    addConstraintMapping(std::string("bool_xor"), CpOptConstraints::p_bool_xor);

    addConstraintMapping(std::string("int_lt"), CpOptConstraints::p_lt);
    addConstraintMapping(std::string("int_lt_reif"), CpOptConstraints::p_lt_reif);

    addConstraintMapping(std::string("bool_lt"), CpOptConstraints::p_lt);
    addConstraintMapping(std::string("bool_lt_reif"), CpOptConstraints::p_lt_reif);
    addConstraintMapping(std::string("int_div"),CpOptConstraints::p_div);
    addConstraintMapping(std::string("int_min"),CpOptConstraints::p_min);
    addConstraintMapping(std::string("int_max"),CpOptConstraints::p_max);
    addConstraintMapping(std::string("int_mod"),CpOptConstraints::p_mod);
    addConstraintMapping(std::string("int_times"), CpOptConstraints::p_times);
    addConstraintMapping(std::string("array_bool_element"), CpOptConstraints::p_array_element);
    addConstraintMapping(std::string("array_var_bool_element"), CpOptConstraints::p_array_var_element);
    addConstraintMapping(std::string("array_int_element"), CpOptConstraints::p_array_element);
    addConstraintMapping(std::string("array_var_int_element"), CpOptConstraints::p_array_var_element);
    addConstraintMapping(std::string("bool_clause"),CpOptConstraints::p_bool_clause);

  }

  void CpOptInterface::solve(SolveI* s) {
    if (s->_st != SolveI::SolveType::ST_SAT) {
      IloObjective obj;
      if (s->_st == SolveI::SolveType::ST_MAX)
	obj = IloMaximize(env);
      else
	obj = IloMinimize(env);
      IloNumVar* v = (IloNumVar*) resolveVar(s->_e);
      obj.setLinearCoef(*v, 1);
      model->add(obj);
    }

    IloCP cplex(*model);
    try{
      if (!cplex.solve()) {
	std::cerr << "Failed to optimize LP" << std::endl;
	return;
      }
    } catch(IloCP::Exception& e){
      std::cerr << "Caught IloCP::Exception while solving : " << std::endl
		<< e << std::endl;
      std::exit(0);
    }
   
    std::cout << "Solution status = " << cplex.getStatus() << std::endl;
    std::cout << "Solution value  = " << cplex.getObjValue() << std::endl;
    std::cout << showVariables(cplex);

  }
  std::string CpOptInterface::showVariable(IloCP& cplex, IloNumVar& v) {
    std::ostringstream oss;
    try {
      if(cplex.isExtracted(v) && cplex.isFixed(v))
	oss << cplex.getValue(v);
      else oss << "_";
    } catch (IloAlgorithm::NotExtractedException& e) {
      oss << "_";
    }
    return oss.str();
  }
  std::string CpOptInterface::showVariables(IloCP& cplex){
    ASTContext context;
    std::ostringstream oss;
    std::map<VarDecl*, void*>::iterator it;
    bool output;
    for(it = variableMap.begin(); it != variableMap.end(); it++){
      output = false;
      Annotation* ann = it->first->_ann;
      ArrayLit* al_dims = NULL;
      while(ann){
	if(ann->_e->isa<Id>() && ann->_e->cast<Id>()->_v.str() =="output_var"){
	  output = true;
	  break;
	} else if (ann->_e->isa<Call>() && 
		   ann->_e->cast<Call>()->_id.str() == "output_array"){
	  al_dims = (*(ann->_e->cast<Call>()->_args))[0]->cast<ArrayLit>();
	  output = true;
	  break;
	}
	ann = ann->_ann;
      }
      if(!output) continue;
      oss <<  it->first->_id.str() << " = ";

      
      if(it->first->_ti->isarray()){
	IloNumVarArray* varray = static_cast<IloNumVarArray*>(it->second);
	int sizeDims = al_dims->_v->size();
	oss << "array" << sizeDims << "d(";
	int size = varray->getSize();	
	for(Expression* e : *(al_dims->_v)){
	  BinOp* bo = e->cast<BinOp>();
	  oss << getNumber<int,IntLit>(bo->_e0) << ".." 
	      << getNumber<int,IntLit>(bo->_e1) << ", ";
	}
	oss << "[";
	for(int i = 0; i < size; i++){
	  if(i!=0)oss << ", ";
	  IloNumVar& v = (*varray)[i];
	  try{
	    if(cplex.isExtracted(v) && cplex.isFixed(v)){
	      IloNum num = cplex.getValue(v);
	      oss << num;
	    } else oss << "_";
	  } catch(IloAlgorithm::NotExtractedException& e){
	    oss << "_";
	  }
	}
	oss << "]);";
      } else {
	oss << showVariable(cplex,*(IloNumVar*)(it->second));
      }
      oss << std::endl;     
    }
    return oss.str();
  }


  CpOptInterface::~CpOptInterface() {
    model->end();
    delete model;
    env.end();
  }
  void* CpOptInterface::getModel() {
    return (void*) (model);
  }
  void* CpOptInterface::addSolverVar(VarDecl* vd) {
    MiniZinc::TypeInst* ti = vd->_ti;
    IloNumVar::Type type;
    switch (ti->_type._bt) {
    case Type::BT_INT:
      type = ILOINT;
      break;
    case Type::BT_BOOL:
      type = ILOBOOL;
      break;
    default:
      std::cerr << "This type of var is not handled by CP Optimizer." << std::endl;
      std::exit(-1);
    }
    Expression* domain = ti->_domain;
    IloNum lb, ub;
    if (domain) {
      std::pair<int,int> bounds;
      try{
	if (type == ILOINT) {
	  bounds = getIntBounds(domain);
	}
	lb = IloNum(bounds.first);
	ub = IloNum(bounds.second);
      }catch(int e){
	lb = -IloInfinity;
	ub = IloInfinity;
      }
    } else {
      lb = -IloInfinity;
      ub = IloInfinity;
    }
    if (ti->isarray()) {
      assert(ti->_ranges->size() == 1);
      Expression* range = (*(ti->_ranges))[0];
      std::pair<int, int> rangebounds;
      rangebounds = getIntBounds(range);
      int rangesize = rangebounds.second - rangebounds.first;

      IloNumVarArray* res = new IloNumVarArray(env, rangesize + 1, lb, ub,
					       type);
      Expression* init = vd->_e;
      if (init) {
	ArrayLit* initarray = init->cast<ArrayLit>();
	CtxVec<Expression*>& ar = *(initarray->_v);
	Expression* f = ar[0];
	if(f->isa<Id>()){
	  switch (type) {
	  case ILOINT:
	    initArray<int, IntLit>(*res, ar);
	    break;
	  case ILOBOOL:
	    initArray<bool, BoolLit>(*res, ar);
	    break;
	  }
	  return (void*)res;
	} else {
	  IloNumExprArray* res2 = new IloNumExprArray(env, rangesize + 1);
	  switch (type) {
	  case ILOINT:
	    initArray<int, IntLit>(*res2, ar);
	    break;
	  case ILOBOOL:
	    initArray<bool, BoolLit>(*res2, ar);
	    break;
	  }
	  return (void*)res2;
	}

      } return (void*)res;
    } else {
      IloNumVar* var = NULL;
      if (vd->_e) {
	Expression* init = vd->_e;

	if (init->isa<Id>()) {
	  var = (IloNumVar*) (resolveVar(init));
	} else {
	  switch (type) {
	  case ILOINT:
	    lb = getNumber<int,IntLit>(init);
	    ub = lb;
	    break;
	  case ILOBOOL:
	    lb = getNumber<bool,BoolLit>(init);
	    ub = lb;
	    break;
	  default:
	    std::cerr << "addSolverVar : init : This var has no type." << std::endl;
	    break;
	  }
	}
      }
      IloNumVar* res = new IloNumVar(env, lb, ub, type, vd->_id.c_str());
      if (var) {
	model->add(IloConstraint(*res == *var));
      }
      return (void*) res;
    }
  }
  template<typename S, typename T>
  void CpOptInterface::initArray(IloNumExprArray& res, CtxVec<Expression*>& ar) {
    for (unsigned int i = 0; i < ar.size(); i++) {
      IloNumExpr* v = (IloNumExpr*)resolveVar(ar[i]);
      res[i] = *v;
    }
  }
  template<typename S, typename T>
  void CpOptInterface::initArray(IloNumVarArray& res, CtxVec<Expression*>& ar) {
    for (unsigned int i = 0; i < ar.size(); i++) {
      IloNumVar* v = (IloNumVar*)resolveVar(ar[i]);
      model->add(IloConstraint(res[i] == *v));
    }
  }
}
;
