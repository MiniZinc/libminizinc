/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Pierre WILKE (wilke.pierre@gmail.com)
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/ast.hh>
#include <minizinc/solver_interface/cplex_interface.hh>
#include <ilcplex/ilocplex.h>
#include <minizinc/printer.hh>

namespace MiniZinc {
  
  namespace CplexConstraints{
    template<typename T, typename S>
    void p_lin(SolverInterface& si, const Call* call,
	       CplexInterface::LIN_CON_TYPE lt, bool reif = false) {
      IloModel* model = (IloModel*) (si.getModel());
      CtxVec<Expression*>& args = *(call->_args);
      CtxVec<Expression*> *coeff = args[0]->cast<ArrayLit>()->_v;
      CtxVec<Expression*> *vars = args[1]->cast<ArrayLit>()->_v;
      T res = getNumber<T, S>(args[2]);

      IloNum lb, ub;
      lb = -IloInfinity;
      ub = IloInfinity;
      switch (lt) {
      case CplexInterface::LQ:
	ub = res;
	break;
      case CplexInterface::GQ:
	lb = res;
	break;
      case CplexInterface::EQ:
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
	IloExpr* varr = (IloExpr*) (si.resolveVar(args[2]));
	model->add(IloConstraint(range == *varr));
      }
      model->add(range);
    }

    void p_array_bool_and(SolverInterface& si, const Call* call) {
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
	    constraint == constraint
	      && IloConstraint(*array_element == true);
	  }
	}
      } else if (args[0]->isa<Id>()) {
	IloExprArray* array = (IloExprArray*) si.resolveVar(args[0]);

	for (unsigned int i = 0; i < array->getSize(); i++) {
	  IloExpr* array_element = (IloExpr*) (&(*array)[i]);
	  if (i == 0) {
	    constraint = IloConstraint(*array_element == true);
	  } else {
	    constraint = constraint
	      && IloConstraint(*array_element == true);
	  }
	}
      }

      model->add(IloConstraint(*var == constraint));
    }

    void p_bool_lin(SolverInterface& si, const Call* call,
		    CplexInterface::LIN_CON_TYPE lt, bool reif = false) {
      p_lin<bool, BoolLit>(si, call, lt, reif);
    }
    void p_bool_lin_le(SolverInterface& si, const Call* call) {
      p_bool_lin(si, call, CplexInterface::LQ);
    }

    void p_bool_lin_eq(SolverInterface& si, const Call* call) {
      p_bool_lin(si, call, CplexInterface::EQ);
    }
    void p_int_lin(SolverInterface& si, const Call* call,
		   CplexInterface::LIN_CON_TYPE lt, bool reif = false) {
      p_lin<int, IntLit>(si, call, lt, reif);
    }
    void p_int_lin_le(SolverInterface& si, const Call* call,
		      bool reif = false) {
      p_int_lin(si, call, CplexInterface::LQ, reif);
    }
    void p_int_lin_le_noreif(SolverInterface& si, const Call* call) {
      p_int_lin_le(si, call);
    }
    void p_int_lin_le_reif(SolverInterface& si, const Call* call) {
      p_int_lin_le(si, call, true);
    }
    void p_int_lin_eq(SolverInterface& si, const Call* call,
		      bool reif = false) {
      p_int_lin(si, call, CplexInterface::EQ, reif);
    }
    void p_int_lin_eq_noreif(SolverInterface& si, const Call* call) {
      p_int_lin_eq(si, call);
    }
    void p_int_lin_eq_reif(SolverInterface& si, const Call* call) {
      p_int_lin_eq(si, call, true);
    }
    void p_float_lin(SolverInterface& si, const Call* call,
		     CplexInterface::LIN_CON_TYPE lt, bool reif = false) {
      p_lin<double, FloatLit>(si, call, lt, reif);
    }
    void p_float_lin_le(SolverInterface& si, const Call* call,
			bool reif = false) {
      p_float_lin(si, call, CplexInterface::LQ, reif);
    }
    void p_float_lin_le_reif(SolverInterface& si, const Call* call) {
      p_float_lin_le(si, call, true);
    }
    void p_float_lin_le_noreif(SolverInterface& si,
			       const Call* call) {
      p_float_lin_le(si, call);
    }
    void p_float_lin_eq(SolverInterface& si, const Call* call,
			bool reif = false) {
      p_float_lin(si, call, CplexInterface::EQ, reif);
    }
    void p_float_lin_eq_reif(SolverInterface& si, const Call* call) {
      p_float_lin_eq(si, call, true);
    }
    void p_float_lin_eq_noreif(SolverInterface& si,
			       const Call* call) {
      p_float_lin_eq(si, call);
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
    void p_int2float(SolverInterface& si, const Call* call) {
      p_eq(si, call);
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
  }
  
  CplexInterface::CplexInterface() {
    model = new IloModel(env);
    addConstraintMapping(std::string("int2float"), CplexConstraints::p_eq);
    addConstraintMapping(std::string("int_abs"), CplexConstraints::p_abs);
    addConstraintMapping(std::string("int_eq"), CplexConstraints::p_eq);
    addConstraintMapping(std::string("int_eq_reif"), CplexConstraints::p_eq_reif);
    addConstraintMapping(std::string("int_le"), CplexConstraints::p_le);
    addConstraintMapping(std::string("int_le_reif"), CplexConstraints::p_le_reif);
    addConstraintMapping(std::string("int_lin_eq"), CplexConstraints::p_int_lin_eq_noreif); //
    addConstraintMapping(std::string("int_lin_eq_reif"), CplexConstraints::p_int_lin_eq_reif);
    addConstraintMapping(std::string("int_lin_le"), CplexConstraints::p_int_lin_le_noreif); //
    addConstraintMapping(std::string("int_lin_le_reif"), CplexConstraints::p_int_lin_le_reif);
    addConstraintMapping(std::string("int_ne"), CplexConstraints::p_ne);
    addConstraintMapping(std::string("int_ne_reif"), CplexConstraints::p_ne_reif);
    addConstraintMapping(std::string("int_plus"), CplexConstraints::p_plus);
    addConstraintMapping(std::string("int_times_le"), CplexConstraints::p_times_le);
    addConstraintMapping(std::string("float_times_le"), CplexConstraints::p_times_le); // must be convex
    addConstraintMapping(std::string("array_bool_and"), CplexConstraints::p_array_bool_and);
    addConstraintMapping(std::string("bool2int"), CplexConstraints::p_eq);
    addConstraintMapping(std::string("bool_and"), CplexConstraints::p_bool_and);
    addConstraintMapping(std::string("bool_eq"), CplexConstraints::p_eq);
    addConstraintMapping(std::string("bool_eq_reif"), CplexConstraints::p_eq_reif);
    addConstraintMapping(std::string("bool_le"), CplexConstraints::p_le);
    addConstraintMapping(std::string("bool_le_reif"), CplexConstraints::p_le_reif);
    addConstraintMapping(std::string("bool_lin_eq"), CplexConstraints::p_bool_lin_eq);
    addConstraintMapping(std::string("bool_lin_le"), CplexConstraints::p_bool_lin_le);
    addConstraintMapping(std::string("bool_not"), CplexConstraints::p_bool_not);
    addConstraintMapping(std::string("bool_or"), CplexConstraints::p_bool_or);
    addConstraintMapping(std::string("bool_xor"), CplexConstraints::p_bool_xor);
    addConstraintMapping(std::string("float_abs"), CplexConstraints::p_abs);
    addConstraintMapping(std::string("float_eq"), CplexConstraints::p_eq);
    addConstraintMapping(std::string("float_eq_reif"), CplexConstraints::p_eq_reif);
    addConstraintMapping(std::string("float_le"), CplexConstraints::p_le);
    addConstraintMapping(std::string("float_le_reif"), CplexConstraints::p_le_reif);
    addConstraintMapping(std::string("float_lin_eq"), CplexConstraints::p_float_lin_eq_noreif);
    addConstraintMapping(std::string("float_lin_eq_reif"), CplexConstraints::p_float_lin_eq_reif);
    addConstraintMapping(std::string("float_lin_le"), CplexConstraints::p_float_lin_le_noreif);
    addConstraintMapping(std::string("float_lin_le_reif"), CplexConstraints::p_float_lin_le_reif);
    addConstraintMapping(std::string("float_ne"), CplexConstraints::p_ne);
    addConstraintMapping(std::string("float_ne_reif"), CplexConstraints::p_ne_reif);
    addConstraintMapping(std::string("float_plus"), CplexConstraints::p_plus);

  }

  void CplexInterface::solve(SolveI* s) {
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

    IloCplex cplex(*model);
    try{
      if (!cplex.solve()) {
	std::cerr << "Failed to optimize LP" << std::endl;
	return;
      }
    } catch(IloCplex::Exception& e){
      std::cerr << "Caught IloCplex::Exception while solving : " << std::endl
		<< e << std::endl;
      std::exit(0);
    }
   
    std::cout << "Solution status = " << cplex.getStatus() << std::endl;
    std::cout << "Solution value  = " << cplex.getObjValue() << std::endl;
    std::cout << showVariables(cplex);

  }
  std::string CplexInterface::showVariable(IloCplex& cplex, IloNumVar& v) {
    std::ostringstream oss;
    try {
      oss << cplex.getValue(v);
    } catch (IloAlgorithm::NotExtractedException& e) {
      oss << "_";
    }
    return oss.str();
  }
  std::string CplexInterface::showVariables(IloCplex& cplex){
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
	Location loc;
	int sizeDims = al_dims->_v->size();
	int size = varray->getSize();	
	std::vector<Expression*>* _vec = new std::vector<Expression*>(size);
	std::vector<std::pair<int,int> >* _dims = new std::vector<std::pair<int,int> >(sizeDims);
	std::vector<Expression*>& vec = *_vec;
	auto& dims = *_dims;
	
	for(int i = 0; i < size; i++){
	  IloNumVar& v = (*varray)[i];
	  try{
	    IloNum num = cplex.getValue(v);
	    switch(v.getType()){
	    case ILOINT:
	      vec[i] = IntLit::a(context,loc,(int)num);
	      break;
	    case ILOFLOAT:
	      vec[i] = FloatLit::a(context,loc,(double)num);
	      break;
	    case ILOBOOL:
	      vec[i] = BoolLit::a(context,loc,(bool)num);
	      break;
	    default:
	      std::cerr << "Wrong type of var" << std::endl;
	    }
	  } catch(IloAlgorithm::NotExtractedException& e){
	    vec[i] = StringLit::a(context, loc, "_");
	  }
	}
	for(int i = 0; i < sizeDims; i++){
	  BinOp* bo = (*al_dims->_v)[i]->cast<BinOp>();
	  assert(bo->_op == BOT_DOTDOT);
	  int lb, ub;
	  lb = getNumber<int,IntLit>(bo->_e0);
	  ub = getNumber<int,IntLit>(bo->_e1);
	   
	  dims[i] = std::pair<int,int>(lb,ub);
	}
	ArrayLit* al = ArrayLit::a(context,loc,vec, dims);
	oss << std::endl << al;
      } else {
	oss << showVariable(cplex,*(IloNumVar*)(it->second));
      }
      oss << std::endl;     
    }
    return oss.str();
  }


  CplexInterface::~CplexInterface() {
    model->end();
    delete model;
    env.end();
  }
  void* CplexInterface::getModel() {
    return (void*) (model);
  }
  void* CplexInterface::addSolverVar(VarDecl* vd) {
    MiniZinc::TypeInst* ti = vd->_ti;
    IloNumVar::Type type;
    switch (ti->_type._bt) {
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
      std::cerr << "This type of var is not handled by CPLEX." << std::endl;
      std::exit(-1);
    }
    Expression* domain = ti->_domain;
    IloNum lb, ub;
    if (domain) {
      std::pair<double, double> bounds;
      if (type == ILOFLOAT) {
	bounds = getFloatBounds(domain);
      } else if (type == ILOINT) {
	bounds = getIntBounds(domain);
      }
      lb = IloNum(bounds.first);
      ub = IloNum(bounds.second);
    } else {
      lb = -IloInfinity;
      ub = IloInfinity;
    }
    if (ti->isarray()) {
      assert(ti->_ranges->size() == 1);
      Expression* range = (*(ti->_ranges))[0];
      std::pair<int, int> rangebounds = getIntBounds(range);
      int rangesize = rangebounds.second - rangebounds.first;
      IloNumVarArray* res = new IloNumVarArray(env, rangesize + 1, lb, ub,
					       type);
      Expression* init = vd->_e;
      if (init) {
	ArrayLit* initarray = init->cast<ArrayLit>();
	CtxVec<Expression*>& ar = *(initarray->_v);
	switch (type) {
	case ILOINT:
	  initArray<int, IntLit>(*res, ar);
	  break;
	case ILOFLOAT:
	  initArray<double, FloatLit>(*res, ar);
	  break;
	case ILOBOOL:
	  initArray<bool, BoolLit>(*res, ar);
	  break;
	}
      }
      return (void*) res;
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
	  case ILOFLOAT:
	    lb = getNumber<float,FloatLit>(init);
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
  void CplexInterface::initArray(IloNumVarArray& res, CtxVec<Expression*>& ar) {
    for (unsigned int i = 0; i < ar.size(); i++) {
      IloNumVar* v = (IloNumVar*)resolveVar(ar[i]);
      model->add(IloConstraint(res[i] == *v));
    }
  }
}
;
