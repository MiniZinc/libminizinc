/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Pierre WILKE (wilke.pierre@gmail.com)
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/ast.hh>
#include <minizinc/solver_interface/cpopt_interface.hh>
#include <ilcp/cp.h>
#include <ilcp/cpext.h>
#include <minizinc/printer.hh>
#include <ilconcert/ilosmodel.h>
namespace MiniZinc {
  
  namespace CpOptConstraints{

    IloIntArray toIntArray(IloExprArray* a, const IloEnv& env){
      IloIntArray values(env);
      unsigned int size = a->getSize();
      for(unsigned int i = 0; i < size; i++){
	IloExpr e = (*a)[i];
	values.add(e.getConstant());
      }
      return values;
    }
    template<typename T, typename S>
    void p_lin(SolverInterface& si, const Call* call,
	       CpOptInterface::LIN_CON_TYPE lt, bool reif = false, bool mustBe = true) {
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
	model->add(IloConstraint((range == *varr) == mustBe));
      } else model->add(range == mustBe);
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
		   CpOptInterface::LIN_CON_TYPE lt, bool reif = false, bool mustBe = true) {
      p_lin<int, IntLit>(si, call, lt, reif, mustBe);
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
		      bool reif = false, bool mustBe = true) {
      p_int_lin(si, call, CpOptInterface::EQ, reif, mustBe);
    }
    void p_int_lin_eq_noreif(SolverInterface& si, const Call* call) {
      p_int_lin_eq(si, call);
    }
    void p_int_lin_eq_reif(SolverInterface& si, const Call* call) {
      p_int_lin_eq(si, call, true);
    }
    void p_int_lin_ne_noreif(SolverInterface& si, const Call* call) {
      p_int_lin_eq(si, call, false, false);
    }
    void p_int_lin_ne_reif(SolverInterface& si, const Call* call) {
      p_int_lin_eq(si, call, true, false);
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
      //std::cout << *var_index << " -- " << *var_array << " -- " << *var_res << std::endl;

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
    void p_no_overlap(SolverInterface& si, const Call* call){
      CtxVec<Expression*>& args = *(call->_args);
      IloExprArray* startTimes = (IloExprArray*) (si.resolveVar(args[0]));
      IloExprArray* durations = (IloExprArray*) (si.resolveVar(args[1]));
      IloModel* model = (IloModel*)(si.getModel());
      IloIntervalVarArray array(model->getEnv());
      unsigned int size = startTimes->getSize();
      for(unsigned int i = 0; i < size; i++){
	IloExpr d = ((*durations)[i]);
	IloIntervalVar ivar(model->getEnv());
	model->add(IloStartOf(ivar) == (*startTimes)[i]);
	model->add(IloEndOf(ivar) == (*startTimes)[i]+d);
	array.add(ivar);
      }
      model->add(IloNoOverlap(model->getEnv(), array));
    }
    void p_cumul(SolverInterface& si, const Call* call){
      CtxVec<Expression*>& args = *(call->_args);
      IloIntVarArray* startTimes = (IloIntVarArray*) (si.resolveVar(args[0]));
      IloNumExprArray* resources = (IloNumExprArray*) (si.resolveVar(args[2]));
      IloNumExprArray* durations = (IloNumExprArray*) (si.resolveVar(args[1]));

      IloExpr* capacity = (IloExpr*) (si.resolveVar(args[3]));
      unsigned int size = startTimes->getSize();
      IloModel* model = (IloModel*)(si.getModel());
      IloIntervalVarArray ivar(model->getEnv(),size);
      IloCumulFunctionExpr res(model->getEnv());
      for (unsigned int i=0; i<size; i++)  {
	ivar[i] = IloIntervalVar(model->getEnv());
	IloIntVar s = (*startTimes)[i];
	IloExpr d = (*durations)[i];
	IloExpr r = (*resources)[i];
	model->add(IloStartOf(ivar[i]) == s);
	model->add(IloLengthOf(ivar[i]) == d);
	res += IloPulse(ivar[i],r.getConstant());  
      }
      model->add(res <= (IloInt)capacity->getConstant());
    }
    void p_alldifferent(SolverInterface& si, const Call* call){
      CtxVec<Expression*>& args = *(call->_args);
      IloIntVarArray* vars = (IloIntVarArray*) (si.resolveVar(args[0]));
      IloModel* model = (IloModel*)(si.getModel());
      model->add(IloAllDiff(model->getEnv(),*vars));
    }
    void p_distribute(SolverInterface& si, const Call* call){
      CtxVec<Expression*>& args = *(call->_args);
      IloIntVarArray* vars = (IloIntVarArray*) (si.resolveVar(args[0]));
      IloExprArray* _values = (IloExprArray*) (si.resolveVar(args[1]));
      IloIntVarArray* cards = (IloIntVarArray*) (si.resolveVar(args[2]));
      IloModel* model = (IloModel*)(si.getModel());
      IloIntArray values = toIntArray(_values,model->getEnv());
      model->add(IloDistribute(model->getEnv(),*cards,values,*vars));
    }
    void p_pack(SolverInterface& si, const Call* call){
      CtxVec<Expression*>& args = *(call->_args);
      IloModel* model = (IloModel*)(si.getModel());
      IloIntVarArray* load = (IloIntVarArray*) (si.resolveVar(args[0]));
      IloIntVarArray* where = (IloIntVarArray*) (si.resolveVar(args[1]));
      IloIntArray weight = toIntArray(
				      ( static_cast<IloExprArray*>(si.resolveVar(args[2])) ),
				      model->getEnv()
				      );
      model->add(IloPack(model->getEnv(),*load,*where,weight));
    }
    // IloCount will only work if the value looked for in the array is fixed.
    void p_count(SolverInterface& si, const Call* call, std::function<IloConstraint(IloNumExprArg*,IloNumExprArg*)> op){
      CtxVec<Expression*>& args = *(call->_args);
      IloModel* model = (IloModel*)(si.getModel());
      IloIntVarArray* array = (IloIntVarArray*) (si.resolveVar(args[0]));
      IloExpr* value = (IloExpr*) (si.resolveVar(args[1]));
      IloExpr* ref = (IloExpr*) (si.resolveVar(args[2]));
      IloNumExpr count = IloCount(*array,value->getConstant());
      model->add( op(&count, ref));
    }
    void p_count_eq(SolverInterface& si, const Call* call){
      p_count(si,call,[](IloNumExprArg* a, IloNumExprArg* b){  return (*a == *b);} );
    }
    void p_count_geq(SolverInterface& si, const Call* call){
      p_count(si,call,[](IloNumExprArg* a, IloNumExprArg* b){ return (*a >= *b);} );
    }
    void p_count_leq(SolverInterface& si, const Call* call){
      p_count(si,call,[](IloNumExprArg* a, IloNumExprArg* b){ return (*a <= *b);} );
    }
    void p_count_gt(SolverInterface& si, const Call* call){
      p_count(si,call,[](IloNumExprArg* a, IloNumExprArg* b){ return (*a > *b);} );
    }
    void p_count_lt(SolverInterface& si, const Call* call){
      p_count(si,call,[](IloNumExprArg* a, IloNumExprArg* b){ return (*a < *b);} );
    }
    void p_count_neq(SolverInterface& si, const Call* call){
      p_count(si,call,[](IloNumExprArg* a, IloNumExprArg* b){ return (*a != *b);} );
    }
    
  }
  CpOptInterface::CpOptInterface() {
    model = new IloModel(env);

    addConstraintMapping(std::string("array_bool_and"), CpOptConstraints::p_array_bool_and);
    addConstraintMapping(std::string("array_bool_element"), CpOptConstraints::p_array_element);
    addConstraintMapping(std::string("array_bool_or"), CpOptConstraints::p_array_bool_or);
    addConstraintMapping(std::string("array_int_element"), CpOptConstraints::p_array_element);
    addConstraintMapping(std::string("array_var_int_element"), CpOptConstraints::p_array_var_element);
    addConstraintMapping(std::string("array_var_bool_element"), CpOptConstraints::p_array_var_element);

    addConstraintMapping(std::string("bool2int"), CpOptConstraints::p_eq);
    addConstraintMapping(std::string("bool_and"), CpOptConstraints::p_bool_and);
    addConstraintMapping(std::string("bool_clause"),CpOptConstraints::p_bool_clause);
    addConstraintMapping(std::string("bool_eq"), CpOptConstraints::p_eq);
    addConstraintMapping(std::string("bool_eq_reif"), CpOptConstraints::p_eq_reif);
    addConstraintMapping(std::string("bool_le"), CpOptConstraints::p_le);
    addConstraintMapping(std::string("bool_le_reif"), CpOptConstraints::p_le_reif);
    addConstraintMapping(std::string("bool_lt"), CpOptConstraints::p_lt);
    addConstraintMapping(std::string("bool_lt_reif"), CpOptConstraints::p_lt_reif);
    addConstraintMapping(std::string("bool_not"), CpOptConstraints::p_bool_not);
    addConstraintMapping(std::string("bool_or"), CpOptConstraints::p_bool_or);
    addConstraintMapping(std::string("bool_xor"), CpOptConstraints::p_bool_xor);

    addConstraintMapping(std::string("int_abs"), CpOptConstraints::p_abs);
    addConstraintMapping(std::string("int_div"),CpOptConstraints::p_div);
    addConstraintMapping(std::string("int_eq"), CpOptConstraints::p_eq);
    addConstraintMapping(std::string("int_eq_reif"), CpOptConstraints::p_eq_reif);
    addConstraintMapping(std::string("int_le"), CpOptConstraints::p_le);
    addConstraintMapping(std::string("int_le_reif"), CpOptConstraints::p_le_reif);
    addConstraintMapping(std::string("int_lin_eq"), CpOptConstraints::p_int_lin_eq_noreif); //
    addConstraintMapping(std::string("int_lin_eq_reif"), CpOptConstraints::p_int_lin_eq_reif);
    //int_lin_gt_reif
    addConstraintMapping(std::string("int_lin_le"), CpOptConstraints::p_int_lin_le_noreif); //
    addConstraintMapping(std::string("int_lin_le_reif"), CpOptConstraints::p_int_lin_le_reif);
    //int_lin_lt
    //int_lin_lt_reif
    addConstraintMapping(std::string("int_lin_ne"), CpOptConstraints::p_int_lin_ne_noreif); //
    addConstraintMapping(std::string("int_lin_ne_reif"), CpOptConstraints::p_int_lin_ne_reif);
    
    addConstraintMapping(std::string("int_lt"), CpOptConstraints::p_lt);
    addConstraintMapping(std::string("int_lt_reif"), CpOptConstraints::p_lt_reif);
    addConstraintMapping(std::string("int_min"),CpOptConstraints::p_min);
    addConstraintMapping(std::string("int_max"),CpOptConstraints::p_max);
    addConstraintMapping(std::string("int_mod"),CpOptConstraints::p_mod);
    addConstraintMapping(std::string("int_ne"), CpOptConstraints::p_ne);
    addConstraintMapping(std::string("int_ne_reif"), CpOptConstraints::p_ne_reif);
    addConstraintMapping(std::string("int_times"), CpOptConstraints::p_times);
    addConstraintMapping(std::string("int_plus"), CpOptConstraints::p_plus);
    
    //Ilog CP Optimizer constraints
    addConstraintMapping(std::string("ilogcp_disjunctive"),CpOptConstraints::p_no_overlap);
    addConstraintMapping(std::string("ilogcp_cumulative"),CpOptConstraints::p_cumul);
    addConstraintMapping(std::string("ilogcp_bin_packing_load"),CpOptConstraints::p_pack);  
    addConstraintMapping(std::string("ilogcp_global_cardinality_closed"),CpOptConstraints::p_distribute);

    addConstraintMapping(std::string("all_different_int"),CpOptConstraints::p_alldifferent);

    addConstraintMapping(std::string("ilogcp_count_eq"),CpOptConstraints::p_count_eq);
    addConstraintMapping(std::string("ilogcp_count_geq"),CpOptConstraints::p_count_geq);
    addConstraintMapping(std::string("ilogcp_count_leq"),CpOptConstraints::p_count_leq);
    addConstraintMapping(std::string("ilogcp_count_gt"),CpOptConstraints::p_count_gt);
    addConstraintMapping(std::string("ilogcp_count_lt"),CpOptConstraints::p_count_lt);
    addConstraintMapping(std::string("ilogcp_count_neq"),CpOptConstraints::p_count_neq);
  }
 
  IlcChooseIndex1(InputOrder,  varIndex, IlcIntVar);
  ILCGOAL3(SearchGoal, IlcIntVarArray, vars, std::string, varSel, std::string,valSel){
    IloInt index = -1;
    if(varSel == std::string("input_order")){
      index = InputOrder(vars);
    } else if(varSel == std::string("first_fail")){
      index = IlcChooseMinSizeInt(vars);
    } else if(varSel == std::string("anti_first_fail")){
      index = IlcChooseMaxSizeInt(vars);
    } else if(varSel == std::string("smallest")){
      index = IlcChooseMinMinInt(vars);
    } else if(varSel == std::string("largest")){
      index = IlcChooseMaxMaxInt(vars);
    } else {
      std::cerr << "Unimplemented variable selection : " << varSel << std::endl;
      std::exit(0);
    }
    if(index == -1) return 0;
    IlcIntVar var = vars[index];
    if(valSel == std::string("indomain_min")){
      IlcInt value = var.getMin();
      return IlcOr( IlcAnd( var == value,
			    SearchGoal(getCP(), vars, varSel, valSel)), 
		    IlcAnd( var != value,
			    SearchGoal(getCP(), vars, varSel, valSel)));
    } else if(valSel == std::string("indomain_max")){
      IlcInt value = var.getMax();
      return IlcOr( IlcAnd( var == value,
			    SearchGoal(getCP(), vars, varSel, valSel)), 
		    IlcAnd( var != value,
			    SearchGoal(getCP(), vars, varSel, valSel)));
    } else if(valSel == std::string("indomain_median")){
      IloInt mid = (var.getMax()+var.getMin()) / 2;
      IlcInt value;
      if(var.isInDomain(mid))
	value = mid;
      else {
	IlcInt valueh = var.getNextHigher( mid );
	IlcInt valuel = var.getNextLower( mid );
	value = (valueh - mid > mid - valuel ? valuel : valueh);
      }
      if(var.isInDomain(value))
	return IlcOr( IlcAnd( var == value,
			      SearchGoal(getCP(), vars, varSel, valSel)), 
		      IlcAnd( var != value,
			      SearchGoal(getCP(), vars, varSel, valSel)));
      else
	return 0;
    } else if(valSel == std::string("indomain_split")){
      IloInt value = (var.getMin() + var.getMax())/2;
      return IlcOr( IlcAnd(var <= value,
			   SearchGoal(getCP(),vars,varSel,valSel)),
		    IlcAnd( var > value,
			    SearchGoal(getCP(), vars, varSel, valSel)));      
    } else if(valSel == std::string("indomain_reverse_split")){
      IloInt value = (var.getMin() + var.getMax())/2;
      return IlcOr( IlcAnd( var > value,
			    SearchGoal(getCP(), vars, varSel, valSel)),
		    IlcAnd( var <= value,
			    SearchGoal(getCP(), vars, varSel, valSel))); 
    } else {
      std::cerr << "Unimplemented value selection : " << valSel << std::endl;
      std::exit(0);
    }
  }
  ILOCPGOALWRAPPER3(oSearchGoal, cp, IloIntVarArray, vars, std::string, varSel, std::string,valSel){
    return SearchGoal(cp, cp.getIntVarArray(vars), varSel, valSel);   
  }

  void CpOptInterface::setObjective(SolveI* s){
    IloObjective obj;
    if (s->_st != SolveI::SolveType::ST_SAT) {
      if (s->_st == SolveI::SolveType::ST_MAX)
	obj = IloMaximize(env);
      else
	obj = IloMinimize(env);
      IloNumVar* v = (IloNumVar*) resolveVar(s->_e);
      obj.setLinearCoef(*v, 1);
      model->add(obj);
    } 
  }
  void CpOptInterface::searchGoal(Call* c, IloCP& cp){
    std::string call_id = c->_id.str();
    CtxVec<Expression*>& args = *(c->_args);
    if(call_id == std::string("int_search") || call_id == std::string("bool_search")){
      IloIntVarArray* vars = (IloIntVarArray*)resolveVar(args[0]);
      std::string varSelectId = args[1]->cast<Id>()->_v.str();
      std::string valSelectId = args[2]->cast<Id>()->_v.str();
      //      model->add(*vars);
      cp.startNewSearch(oSearchGoal(env,*vars,varSelectId,valSelectId));
      assert(args[3]->cast<Id>()->_v.str() == std::string("complete"));
    } else if(call_id == std::string("seq_search")){
      ArrayLit* ar = args[0]->cast<ArrayLit>();
      unsigned int size = ar->_v->size();
      for(unsigned int i = 0; i < size; i++){
	Call* s = (*ar->_v)[i]->cast<Call>();
	searchGoal(s,cp);
      }
    } else {
      std::cerr << "Must implement " << call_id << "!" << std::endl;
      std::exit(0);
    }
  }
  void CpOptInterface::solve(SolveI* s) {
    setObjective(s);
    IloCP cplex(*model);
    cplex.setParameter(IloCP::LogVerbosity,IloCP::Quiet);
    cplex.setParameter(IloCP::Workers,nbThreads);
    cplex.setParameter(IloCP::TimeLimit,60);
    cplex.setParameter(IloCP::DefaultInferenceLevel,IloCP::Extended);

    Annotation* ann = s->_ann;
    if(ann)
	  searchGoal(ann->_e->cast<Call>(),cplex);//free ?
    cplex.startNewSearch();
    try{
      unsigned int nbSol = 0;
      while(cplex.next() && (nbSol < 1 || allSolutions)) {
	nbSol++;
	// std::cout << "Solution status : " << cplex.getStatus() << std::endl;
	/*
	if(cplex.hasObjective())
	std::cout << "Solution value  = " << cplex.getObjValue() << std::endl;*/
	std::cout << showVariables(cplex);
	std::cout << "----------" << std::endl;
      }

      if(!cplex.next())
	std::cout << "==========" << std::endl;
      return;
      
    } catch(IloCP::Exception& e){
      std::cerr << "Caught IloCP::Exception while solving : " << std::endl
		<< e << std::endl;
      std::exit(0);
    }
  }
  std::string CpOptInterface::showVariable(IloCP& cplex, IloNumVar& v) {
    std::ostringstream oss;
    try {
      if(cplex.isExtracted(v) && cplex.isFixed(v))
	oss << cplex.getValue(v);
      else oss << v;
    } catch (IloAlgorithm::NotExtractedException& e) {
      oss << v;
    }
    oss << ";";
    return oss.str();
  }
  std::string CpOptInterface::showVariables(IloCP& cplex){
    ASTContext context;
    std::ostringstream oss;
    std::map<VarDecl*, void*>::iterator it;
    bool output;

    for(const auto& item: variableMap){
      VarDecl* vd = item.first;
      void* varptr = item.second;
      output = false;
      if(!vd) continue;
      Annotation* ann = vd->_ann;
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
      oss <<  vd->_id.str() << " = ";

      
      if(vd->_ti->isarray()){
	IloNumVarArray* varray = (IloNumVarArray*)(varptr);
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
	    } else {
	      oss << (IloNumExpr)v;
	    }
	  } catch(IloAlgorithm::NotExtractedException& e){
	    oss << (IloNumExpr)v;
	  }
	}
	oss << "]);";
      } else {
	oss << showVariable(cplex,*(IloNumVar*)(varptr));
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
	  default:
	    break;
	  }
	  model->add(*res);
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
	  default:
	    break;
	  }
	  model->add(*res2);
	  return (void*)res2;
	}

      }
      model->add(*res);
      return (void*)res;
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
      model->add(*res);
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
  // template<typename S, typename T>
  // void CpOptInterface::initArray(IloNumVarArray& res, CtxVec<Expression*>& ar) {
  //   for (unsigned int i = 0; i < ar.size(); i++) {
  //     IloNumVar* v = (IloNumVar*)resolveVar(ar[i]);
  //     std::cout << "initArray var " << *v<<std::endl;
  //     model->add(res[i] == *v);
  //   }
  // }
}
;
