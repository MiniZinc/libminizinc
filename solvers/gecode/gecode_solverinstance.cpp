/*
 *  Main authors:
 *     Kevin Leo <kevin.leo@monash.edu>
 *     Andrea Rendl <andrea.rendl@nicta.com.au>
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/solver_instance_base.hh>
#include <minizinc/exception.hh>
#include <minizinc/ast.hh>
#include <minizinc/eval_par.hh>

#include "gecode_solverinstance.hh"
#include "gecode_constraints.hh"

using namespace Gecode;

namespace MiniZinc {
  
     GecodeSolverInstance::GecodeSolverInstance(Env& env, const Options& options) 
     : SolverInstanceImpl<GecodeSolver>(env,options), _current_space(NULL) {
       registerConstraints(); 
       // processFlatZinc(); // TODO: shouldn't this better be in the constructor?
     }
  
    GecodeSolverInstance::~GecodeSolverInstance(void) {
      //delete _current_space;
    }
    
    void GecodeSolverInstance::registerConstraints(void) {
      _constraintRegistry.add(ASTString("all_different_int"), GecodeConstraints::p_distinct);
      _constraintRegistry.add(ASTString("all_different_offset"), GecodeConstraints::p_distinctOffset);
      _constraintRegistry.add(ASTString("all_equal_int"), GecodeConstraints::p_all_equal);
      _constraintRegistry.add(ASTString("int_eq"), GecodeConstraints::p_int_eq);
      _constraintRegistry.add(ASTString("int_ne"), GecodeConstraints::p_int_ne);
      _constraintRegistry.add(ASTString("int_ge"), GecodeConstraints::p_int_ge);
      _constraintRegistry.add(ASTString("int_gt"), GecodeConstraints::p_int_gt);
      _constraintRegistry.add(ASTString("int_le"), GecodeConstraints::p_int_le);
      _constraintRegistry.add(ASTString("int_lt"), GecodeConstraints::p_int_lt);
      _constraintRegistry.add(ASTString("int_eq_reif"), GecodeConstraints::p_int_eq_reif);
      _constraintRegistry.add(ASTString("int_ne_reif"), GecodeConstraints::p_int_ne_reif);
      _constraintRegistry.add(ASTString("int_ge_reif"), GecodeConstraints::p_int_ge_reif);
      _constraintRegistry.add(ASTString("int_gt_reif"), GecodeConstraints::p_int_gt_reif);
      _constraintRegistry.add(ASTString("int_le_reif"), GecodeConstraints::p_int_le_reif);
      _constraintRegistry.add(ASTString("int_lt_reif"), GecodeConstraints::p_int_lt_reif);
      _constraintRegistry.add(ASTString("int_eq_imp"), GecodeConstraints::p_int_eq_imp);
      _constraintRegistry.add(ASTString("int_ne_imp"), GecodeConstraints::p_int_ne_imp);
      _constraintRegistry.add(ASTString("int_ge_imp"), GecodeConstraints::p_int_ge_imp);
      _constraintRegistry.add(ASTString("int_gt_imp"), GecodeConstraints::p_int_gt_imp);
      _constraintRegistry.add(ASTString("int_le_imp"), GecodeConstraints::p_int_le_imp);
      _constraintRegistry.add(ASTString("int_lt_imp"), GecodeConstraints::p_int_lt_imp);
      _constraintRegistry.add(ASTString("int_lin_eq"), GecodeConstraints::p_int_lin_eq);
      _constraintRegistry.add(ASTString("int_lin_eq_reif"), GecodeConstraints::p_int_lin_eq_reif);
      _constraintRegistry.add(ASTString("int_lin_eq_imp"), GecodeConstraints::p_int_lin_eq_imp);
      _constraintRegistry.add(ASTString("int_lin_ne"), GecodeConstraints::p_int_lin_ne);
      _constraintRegistry.add(ASTString("int_lin_ne_reif"), GecodeConstraints::p_int_lin_ne_reif);
      _constraintRegistry.add(ASTString("int_lin_ne_imp"), GecodeConstraints::p_int_lin_ne_imp);
      _constraintRegistry.add(ASTString("int_lin_le"), GecodeConstraints::p_int_lin_le);
      _constraintRegistry.add(ASTString("int_lin_le_reif"), GecodeConstraints::p_int_lin_le_reif);
      _constraintRegistry.add(ASTString("int_lin_le_imp"), GecodeConstraints::p_int_lin_le_imp);
      _constraintRegistry.add(ASTString("int_lin_lt"), GecodeConstraints::p_int_lin_lt);
      _constraintRegistry.add(ASTString("int_lin_lt_reif"), GecodeConstraints::p_int_lin_lt_reif);
      _constraintRegistry.add(ASTString("int_lin_lt_imp"), GecodeConstraints::p_int_lin_lt_imp);
      _constraintRegistry.add(ASTString("int_lin_ge"), GecodeConstraints::p_int_lin_ge);
      _constraintRegistry.add(ASTString("int_lin_ge_reif"), GecodeConstraints::p_int_lin_ge_reif);
      _constraintRegistry.add(ASTString("int_lin_ge_imp"), GecodeConstraints::p_int_lin_ge_imp);
      _constraintRegistry.add(ASTString("int_lin_gt"), GecodeConstraints::p_int_lin_gt);
      _constraintRegistry.add(ASTString("int_lin_gt_reif"), GecodeConstraints::p_int_lin_gt_reif);
      _constraintRegistry.add(ASTString("int_lin_gt_imp"), GecodeConstraints::p_int_lin_gt_imp);
      _constraintRegistry.add(ASTString("int_plus"), GecodeConstraints::p_int_plus);
      _constraintRegistry.add(ASTString("int_minus"), GecodeConstraints::p_int_minus);
      _constraintRegistry.add(ASTString("int_times"), GecodeConstraints::p_int_times);
      _constraintRegistry.add(ASTString("int_div"), GecodeConstraints::p_int_div);
      _constraintRegistry.add(ASTString("int_mod"), GecodeConstraints::p_int_mod);
      _constraintRegistry.add(ASTString("int_min"), GecodeConstraints::p_int_min);
      _constraintRegistry.add(ASTString("int_max"), GecodeConstraints::p_int_max);
      _constraintRegistry.add(ASTString("int_abs"), GecodeConstraints::p_abs);
      _constraintRegistry.add(ASTString("int_negate"), GecodeConstraints::p_int_negate);
      _constraintRegistry.add(ASTString("bool_eq"), GecodeConstraints::p_bool_eq);
      _constraintRegistry.add(ASTString("bool_eq_reif"), GecodeConstraints::p_bool_eq_reif);
      _constraintRegistry.add(ASTString("bool_eq_imp"), GecodeConstraints::p_bool_eq_imp);
      _constraintRegistry.add(ASTString("bool_ne"), GecodeConstraints::p_bool_ne);
      _constraintRegistry.add(ASTString("bool_ne_reif"), GecodeConstraints::p_bool_ne_reif);
      _constraintRegistry.add(ASTString("bool_ne_imp"), GecodeConstraints::p_bool_ne_imp);
      _constraintRegistry.add(ASTString("bool_ge"), GecodeConstraints::p_bool_ge);
      _constraintRegistry.add(ASTString("bool_ge_reif"), GecodeConstraints::p_bool_ge_reif);
      _constraintRegistry.add(ASTString("bool_ge_imp"), GecodeConstraints::p_bool_ge_imp);
      _constraintRegistry.add(ASTString("bool_le"), GecodeConstraints::p_bool_le);
      _constraintRegistry.add(ASTString("bool_le_reif"), GecodeConstraints::p_bool_le_reif);
      _constraintRegistry.add(ASTString("bool_le_imp"), GecodeConstraints::p_bool_le_imp);
      _constraintRegistry.add(ASTString("bool_gt"), GecodeConstraints::p_bool_gt);
      _constraintRegistry.add(ASTString("bool_gt_reif"), GecodeConstraints::p_bool_gt_reif);
      _constraintRegistry.add(ASTString("bool_gt_imp"), GecodeConstraints::p_bool_gt_imp);
      _constraintRegistry.add(ASTString("bool_lt"), GecodeConstraints::p_bool_lt);
      _constraintRegistry.add(ASTString("bool_lt_reif"), GecodeConstraints::p_bool_lt_reif);
      _constraintRegistry.add(ASTString("bool_lt_imp"), GecodeConstraints::p_bool_lt_imp);
      _constraintRegistry.add(ASTString("bool_or"), GecodeConstraints::p_bool_or);
      _constraintRegistry.add(ASTString("bool_or_imp"), GecodeConstraints::p_bool_or_imp);
      _constraintRegistry.add(ASTString("bool_and"), GecodeConstraints::p_bool_and);
      _constraintRegistry.add(ASTString("bool_and_imp"), GecodeConstraints::p_bool_and_imp);
      _constraintRegistry.add(ASTString("bool_xor"), GecodeConstraints::p_bool_xor);
      _constraintRegistry.add(ASTString("bool_xor_imp"), GecodeConstraints::p_bool_xor_imp);
      _constraintRegistry.add(ASTString("array_bool_and"), GecodeConstraints::p_array_bool_and);
      _constraintRegistry.add(ASTString("array_bool_and_imp"), GecodeConstraints::p_array_bool_and_imp);
      _constraintRegistry.add(ASTString("array_bool_or"), GecodeConstraints::p_array_bool_or);
      _constraintRegistry.add(ASTString("array_bool_or_imp"), GecodeConstraints::p_array_bool_or_imp);
      _constraintRegistry.add(ASTString("array_bool_xor"), GecodeConstraints::p_array_bool_xor);
      _constraintRegistry.add(ASTString("array_bool_xor_imp"), GecodeConstraints::p_array_bool_xor_imp);
      _constraintRegistry.add(ASTString("bool_clause"), GecodeConstraints::p_array_bool_clause);
      _constraintRegistry.add(ASTString("bool_clause_reif"), GecodeConstraints::p_array_bool_clause_reif);
      _constraintRegistry.add(ASTString("bool_clause_imp"), GecodeConstraints::p_array_bool_clause_imp);
      _constraintRegistry.add(ASTString("bool_left_imp"), GecodeConstraints::p_bool_l_imp);
      _constraintRegistry.add(ASTString("bool_right_imp"), GecodeConstraints::p_bool_r_imp);
      _constraintRegistry.add(ASTString("bool_not"), GecodeConstraints::p_bool_not);
      _constraintRegistry.add(ASTString("array_int_element"), GecodeConstraints::p_array_int_element);
      _constraintRegistry.add(ASTString("array_var_int_element"), GecodeConstraints::p_array_int_element);
      _constraintRegistry.add(ASTString("array_bool_element"), GecodeConstraints::p_array_bool_element);
      _constraintRegistry.add(ASTString("array_var_bool_element"), GecodeConstraints::p_array_bool_element);
      _constraintRegistry.add(ASTString("bool2int"), GecodeConstraints::p_bool2int);
      _constraintRegistry.add(ASTString("int_in"), GecodeConstraints::p_int_in);
      _constraintRegistry.add(ASTString("int_in_reif"), GecodeConstraints::p_int_in_reif);
      _constraintRegistry.add(ASTString("int_in_imp"), GecodeConstraints::p_int_in_imp);
      //#ifndef GECODE_HAS_SET_VARS
      _constraintRegistry.add(ASTString("set_in"), GecodeConstraints::p_int_in);
      _constraintRegistry.add(ASTString("set_in_reif"), GecodeConstraints::p_int_in_reif);
      _constraintRegistry.add(ASTString("set_in_imp"), GecodeConstraints::p_int_in_imp);
      //#endif
      _constraintRegistry.add(ASTString("array_int_lt"), GecodeConstraints::p_array_int_lt);
      _constraintRegistry.add(ASTString("array_int_lq"), GecodeConstraints::p_array_int_lq);
      _constraintRegistry.add(ASTString("array_bool_lt"), GecodeConstraints::p_array_bool_lt);
      _constraintRegistry.add(ASTString("array_bool_lq"), GecodeConstraints::p_array_bool_lq);
      _constraintRegistry.add(ASTString("count"), GecodeConstraints::p_count);
      _constraintRegistry.add(ASTString("count_reif"), GecodeConstraints::p_count_reif);
      _constraintRegistry.add(ASTString("count_imp"), GecodeConstraints::p_count_imp);
      _constraintRegistry.add(ASTString("at_least_int"), GecodeConstraints::p_at_least);
      _constraintRegistry.add(ASTString("at_most_int"), GecodeConstraints::p_at_most);
      _constraintRegistry.add(ASTString("gecode_bin_packing_load"), GecodeConstraints::p_bin_packing_load);
      _constraintRegistry.add(ASTString("global_cardinality"), GecodeConstraints::p_global_cardinality);
      _constraintRegistry.add(ASTString("global_cardinality_closed"), GecodeConstraints::p_global_cardinality_closed);
      _constraintRegistry.add(ASTString("global_cardinality_low_up"), GecodeConstraints::p_global_cardinality_low_up);
      _constraintRegistry.add(ASTString("global_cardinality_low_up_closed"), GecodeConstraints::p_global_cardinality_low_up_closed);
      _constraintRegistry.add(ASTString("minimum_int"), GecodeConstraints::p_minimum);
      _constraintRegistry.add(ASTString("maximum_int"), GecodeConstraints::p_maximum);
      //addConstraintMappinASTString(g("regular"), GecodeConstraints::p_regular);
      _constraintRegistry.add(ASTString("sort"), GecodeConstraints::p_sort);
      _constraintRegistry.add(ASTString("inverse_offsets"), GecodeConstraints::p_inverse_offsets);
      _constraintRegistry.add(ASTString("increasing_int"), GecodeConstraints::p_increasing_int);
      _constraintRegistry.add(ASTString("increasing_bool"), GecodeConstraints::p_increasing_bool);
      _constraintRegistry.add(ASTString("decreasing_int"), GecodeConstraints::p_decreasing_int);
      _constraintRegistry.add(ASTString("decreasing_bool"), GecodeConstraints::p_decreasing_bool);
      _constraintRegistry.add(ASTString("table_int"), GecodeConstraints::p_table_int);
      _constraintRegistry.add(ASTString("table_bool"), GecodeConstraints::p_table_bool);
      _constraintRegistry.add(ASTString("cumulatives"), GecodeConstraints::p_cumulatives);
      _constraintRegistry.add(ASTString("gecode_among_seq_int"), GecodeConstraints::p_among_seq_int);
      _constraintRegistry.add(ASTString("gecode_among_seq_bool"), GecodeConstraints::p_among_seq_bool);

      _constraintRegistry.add(ASTString("bool_lin_eq"), GecodeConstraints::p_bool_lin_eq);
      _constraintRegistry.add(ASTString("bool_lin_ne"), GecodeConstraints::p_bool_lin_ne);
      _constraintRegistry.add(ASTString("bool_lin_le"), GecodeConstraints::p_bool_lin_le);
      _constraintRegistry.add(ASTString("bool_lin_lt"), GecodeConstraints::p_bool_lin_lt);
      _constraintRegistry.add(ASTString("bool_lin_ge"), GecodeConstraints::p_bool_lin_ge);
      _constraintRegistry.add(ASTString("bool_lin_gt"), GecodeConstraints::p_bool_lin_gt);

      _constraintRegistry.add(ASTString("bool_lin_eq_reif"), GecodeConstraints::p_bool_lin_eq_reif);
      _constraintRegistry.add(ASTString("bool_lin_eq_imp"), GecodeConstraints::p_bool_lin_eq_imp);
      _constraintRegistry.add(ASTString("bool_lin_ne_reif"), GecodeConstraints::p_bool_lin_ne_reif);
      _constraintRegistry.add(ASTString("bool_lin_ne_imp"), GecodeConstraints::p_bool_lin_ne_imp);
      _constraintRegistry.add(ASTString("bool_lin_le_reif"), GecodeConstraints::p_bool_lin_le_reif);
      _constraintRegistry.add(ASTString("bool_lin_le_imp"), GecodeConstraints::p_bool_lin_le_imp);
      _constraintRegistry.add(ASTString("bool_lin_lt_reif"), GecodeConstraints::p_bool_lin_lt_reif);
      _constraintRegistry.add(ASTString("bool_lin_lt_imp"), GecodeConstraints::p_bool_lin_lt_imp);
      _constraintRegistry.add(ASTString("bool_lin_ge_reif"), GecodeConstraints::p_bool_lin_ge_reif);
      _constraintRegistry.add(ASTString("bool_lin_ge_imp"), GecodeConstraints::p_bool_lin_ge_imp);
      _constraintRegistry.add(ASTString("bool_lin_gt_reif"), GecodeConstraints::p_bool_lin_gt_reif);
      _constraintRegistry.add(ASTString("bool_lin_gt_imp"), GecodeConstraints::p_bool_lin_gt_imp);

      _constraintRegistry.add(ASTString("gecode_schedule_unary"), GecodeConstraints::p_schedule_unary);
      _constraintRegistry.add(ASTString("gecode_schedule_unary_optional"), GecodeConstraints::p_schedule_unary_optional);

      _constraintRegistry.add(ASTString("gecode_circuit"), GecodeConstraints::p_circuit);
      _constraintRegistry.add(ASTString("gecode_circuit_cost_array"), GecodeConstraints::p_circuit_cost_array);
      _constraintRegistry.add(ASTString("gecode_circuit_cost"), GecodeConstraints::p_circuit_cost);
      _constraintRegistry.add(ASTString("gecode_nooverlap"), GecodeConstraints::p_nooverlap);
      _constraintRegistry.add(ASTString("gecode_precede"), GecodeConstraints::p_precede);
      _constraintRegistry.add(ASTString("nvalue"), GecodeConstraints::p_nvalue);
      _constraintRegistry.add(ASTString("among"), GecodeConstraints::p_among);
      _constraintRegistry.add(ASTString("member_int"), GecodeConstraints::p_member_int);
      _constraintRegistry.add(ASTString("gecode_member_int_reif"), GecodeConstraints::p_member_int_reif);
      _constraintRegistry.add(ASTString("member_bool"), GecodeConstraints::p_member_bool);
      _constraintRegistry.add(ASTString("gecode_member_bool_reif"), GecodeConstraints::p_member_bool_reif);

#ifdef GECODE_HAS_FLOAT_VARS
      _constraintRegistry.add(ASTString("int2float"),GecodeConstraints::p_int2float);
      _constraintRegistry.add(ASTString("float_abs"),GecodeConstraints::p_float_abs);
      _constraintRegistry.add(ASTString("float_sqrt"),GecodeConstraints::p_float_sqrt);
      _constraintRegistry.add(ASTString("float_eq"),GecodeConstraints::p_float_eq);
      _constraintRegistry.add(ASTString("float_eq_reif"),GecodeConstraints::p_float_eq_reif);
      _constraintRegistry.add(ASTString("float_le"),GecodeConstraints::p_float_le);
      _constraintRegistry.add(ASTString("float_le_reif"),GecodeConstraints::p_float_le_reif);
      _constraintRegistry.add(ASTString("float_lt"),GecodeConstraints::p_float_lt);
      _constraintRegistry.add(ASTString("float_lt_reif"),GecodeConstraints::p_float_lt_reif);
      _constraintRegistry.add(ASTString("float_ne"),GecodeConstraints::p_float_ne);
      _constraintRegistry.add(ASTString("float_times"),GecodeConstraints::p_float_times);
      _constraintRegistry.add(ASTString("float_div"),GecodeConstraints::p_float_div);
      _constraintRegistry.add(ASTString("float_plus"),GecodeConstraints::p_float_plus);
      _constraintRegistry.add(ASTString("float_max"),GecodeConstraints::p_float_max);
      _constraintRegistry.add(ASTString("float_min"),GecodeConstraints::p_float_min);
      _constraintRegistry.add(ASTString("float_lin_eq"),GecodeConstraints::p_float_lin_eq);
      _constraintRegistry.add(ASTString("float_lin_eq_reif"),GecodeConstraints::p_float_lin_eq_reif);
      _constraintRegistry.add(ASTString("float_lin_le"),GecodeConstraints::p_float_lin_le);
      _constraintRegistry.add(ASTString("float_lin_le_reif"),GecodeConstraints::p_float_lin_le_reif);
#endif
#ifdef GECODE_HAS_MPFR
      _constraintRegistry.add(ASTString("float_acos"),GecodeConstraints::p_float_acos);
      _constraintRegistry.add(ASTString("float_asin"),GecodeConstraints::p_float_asin);
      _constraintRegistry.add(ASTString("float_atan"),GecodeConstraints::p_float_atan);
      _constraintRegistry.add(ASTString("float_cos"),GecodeConstraints::p_float_cos);       
      _constraintRegistry.add(ASTString("float_exp"),GecodeConstraints::p_float_exp);
      _constraintRegistry.add(ASTString("float_ln"),GecodeConstraints::p_float_ln);
      _constraintRegistry.add(ASTString("float_log10"),GecodeConstraints::p_float_log10);
      _constraintRegistry.add(ASTString("float_log2"),GecodeConstraints::p_float_log2);
      _constraintRegistry.add(ASTString("float_sin"),GecodeConstraints::p_float_sin);       
      _constraintRegistry.add(ASTString("float_tan"),GecodeConstraints::p_float_tan);       
#endif		      
    }
    
    
  void GecodeSolverInstance::processFlatZinc(void) {    
    _current_space = new FznSpace(); 
    
    // iterate over VarDecls of the flat model and create variables
    for (VarDeclIterator it = _env.flat()->begin_vardecls(); it != _env.flat()->end_vardecls(); ++it) {
      if (it->e()->type().isvar()) {
        if (it->e()->type().dim() != 0) {
          // we ignore arrays - all their elements are defined
          continue;
        }
        MiniZinc::TypeInst* ti = it->e()->ti();  
        bool isDefined, isIntroduced = false;
        switch(ti->type().bt()) {
          
          case Type::BT_INT:            
            if(!it->e()->e()) { // if there is no initialisation expression
                Expression* domain = ti->domain();                
                if(domain) {
                    if(domain->isa<SetLit>()) {
                        IntVar intVar(*this->_current_space, arg2intset(domain));
                        _current_space->iv.push_back(intVar);
                        _variableMap.insert(it->e()->id(), 
                                            GecodeVariable(intVar));
                    } else {                                      
                        std::pair<double,double> bounds = getIntBounds(domain); 
                        int lb = bounds.first;
                        int ub = bounds.second;  
                        IntVar intVar(*this->_current_space, lb, ub);
                        _current_space->iv.push_back(intVar);    
                        _variableMap.insert(it->e()->id(), GecodeVariable(intVar));
                    }
                } else {
                    int lb = Gecode::Int::Limits::min;
                    int ub = Gecode::Int::Limits::max;
                    IntVar intVar(*this->_current_space, lb, ub);
                    _current_space->iv.push_back(intVar);
                    _variableMap.insert(it->e()->id(), GecodeVariable(intVar));
                }
            } else { // there is an initialisation expression
                Expression* init = it->e()->e();                
                if (init->isa<Id>() || init->isa<ArrayAccess>()) {
                   // root->iv[root->intVarCount++] = root->iv[*(int*)resolveVar(init)];                                      
                   GecodeVariable var = resolveVar(init);
                   assert(var.isint());
                  _current_space->iv.push_back(var.intVar());
                  _variableMap.insert(it->e()->id(), var);                                  
                } else {
                    double il = init->cast<IntLit>()->v().toInt();
                    IntVar intVar(*this->_current_space, il, il);
                    _current_space->iv.push_back(intVar);
                    _variableMap.insert(it->e()->id(), GecodeVariable(intVar));
                }
            }
            isIntroduced = it->e()->introduced() || (MiniZinc::getAnnotation(it->e()->ann(), constants().ann.is_introduced.str()) != NULL);
            _current_space->iv_introduced.push_back(isIntroduced);
            isDefined = MiniZinc::getAnnotation(it->e()->ann(), constants().ann.is_defined_var->str().str()) != NULL;
            _current_space->iv_defined.push_back(isDefined);                    
            break;
            
          case Type::BT_BOOL: 
          {
            double lb=0, ub=1;
            if(!it->e()->e()) { // there is NO initialisation expression
                Expression* domain = ti->domain();
                if(domain) {                  
                    std::pair<double,double> bounds = getIntBounds(domain); 
                    lb = bounds.first;
                    ub = bounds.second;
                } else {
                    lb = 0;
                    ub = 1;
                }
                BoolVar boolVar(*this->_current_space, lb, ub);
                _current_space->bv.push_back(boolVar);
                _variableMap.insert(it->e()->id(), GecodeVariable(boolVar));
            } else { // there is an initialisation expression
                Expression* init = it->e()->e();
                if (init->isa<Id>() || init->isa<ArrayAccess>()) {
                    // root->bv[root->boolVarCount++] = root->bv[*(int*)resolveVar(init)];                  
                    //int index = *(int*) resolveVar(init);
                    GecodeVariable var = resolveVar(init);
                    assert(var.isbool());                    
                    _current_space->bv.push_back(var.boolVar());
                    _variableMap.insert(it->e()->id(), var);                                    
                } else {
                    double b = (double) init->cast<BoolLit>()->v();
                    BoolVar boolVar(*this->_current_space, b, b);
                    _current_space->bv.push_back(boolVar);
                    _variableMap.insert(it->e()->id(), GecodeVariable(boolVar));
                }
            }
            isIntroduced = it->e()->introduced() || (MiniZinc::getAnnotation(it->e()->ann(), constants().ann.is_introduced.str()) != NULL);
            _current_space->bv_introduced.push_back(isIntroduced);
            isDefined = MiniZinc::getAnnotation(it->e()->ann(), constants().ann.is_defined_var->str().str()) != NULL;
            _current_space->bv_defined.push_back(isDefined);                      
            break;
          }
          
          case Type::BT_FLOAT:  
          {
            if(it->e()->e() == NULL) { // there is NO initialisation expression
                Expression* domain = ti->domain();
                double lb, ub;
                if (domain) {                                      
                    std::pair<double,double> bounds = getFloatBounds(domain); 
                    lb = bounds.first;
                    ub = bounds.second;                   
                } else {
                    lb = Gecode::Int::Limits::min;
                    ub = Gecode::Int::Limits::max;
                }
                FloatVar floatVar(*this->_current_space, lb, ub);
                _current_space->fv.push_back(floatVar);
                _variableMap.insert(it->e()->id(), GecodeVariable(floatVar));
            } else {
                Expression* init = it->e()->e();
                if (init->isa<Id>() || init->isa<ArrayAccess>()) {
                    // root->fv[root->floatVarCount++] = root->fv[*(int*)resolveVar(init)];                                      
                    GecodeVariable var = resolveVar(init);
                    assert(var.isfloat());                   
                    _current_space->fv.push_back(var.floatVar());
                    _variableMap.insert(it->e()->id(), var);                          
                } else {
                    double il = init->cast<FloatLit>()->v();
                    FloatVar floatVar(*this->_current_space, il, il);
                    _current_space->fv.push_back(floatVar);
                    _variableMap.insert(it->e()->id(), GecodeVariable(floatVar));
                }
            }
            isIntroduced = it->e()->introduced() || (MiniZinc::getAnnotation(it->e()->ann(), constants().ann.is_introduced.str()) != NULL);
            _current_space->fv_introduced.push_back(isIntroduced);
            isDefined = MiniZinc::getAnnotation(it->e()->ann(), constants().ann.is_defined_var->str().str()) != NULL;
            _current_space->fv_defined.push_back(isDefined);            
          }
          break;                     
            
          default:
            std::stringstream ssm; 
            ssm << "Type " << ti->type().bt() << " is currently not supported by Gecode." 
                << std::endl;
            throw InternalError(ssm.str());        
          
        }                   
      } // end if it is a variable
    } // end for all var decls
    
    // post the constraints
    for (ConstraintIterator it = _env.flat()->begin_constraints(); it != _env.flat()->end_constraints(); ++it) {
      if (Call* c = it->e()->dyn_cast<Call>()) {
        _constraintRegistry.post(c);
      }
    }    
    
    // objective
    SolveI* si = _env.flat()->solveItem();
    _current_space->_solveType = si->st();
    if(si->e()) {
      _current_space->_optVarIsInt = (si->e()->type().isvarint());      
      if(Id* id = si->e()->dyn_cast<Id>()) {
        GecodeVariable var = resolveVar(id->decl());
        if(_current_space->_optVarIsInt) {
          IntVar intVar = var.intVar();
          for(int i=0; i<_current_space->iv.size(); i++) {
            if(&(_current_space->iv[i]) == &intVar) {
              _current_space->_optVarIdx = i;
              break;
            }
          }
          assert(_current_space->_optVarIdx >= 0);
        } else {
          FloatVar floatVar = var.floatVar();
          for(int i=0; i<_current_space->fv.size(); i++) {
            if(&(_current_space->fv[i]) == &floatVar) {
              _current_space->_optVarIdx = i;
              break;
            }
          }
          assert(_current_space->_optVarIdx >= 0);
        }        
      }
      else { // the solve expression has to be a variable/id
        assert(false);
      }
      
    }
   
    
    std::cout << "DEBUG: at end of processFlatZinc: " << std::endl 
              << "iv has " << _current_space->iv.size() << " variables " << std::endl
              << "bv has " << _current_space->bv.size() << " variables " << std::endl
              << "fv has " << _current_space->fv.size() << " variables " << std::endl
              << "sv has " << _current_space->sv.size() << " variables " << std::endl;              
  }
  
  Gecode::IntArgs 
  GecodeSolverInstance::arg2intargs(Expression* arg, int offset) {
    if(!arg->isa<Id>() && !arg->isa<ArrayLit>()) {
      std::stringstream ssm; ssm << "Invalid argument in arg2intargs: " << *arg;
      ssm << ". Expected Id or ArrayLit.";
      throw InternalError(ssm.str());
    }
    ArrayLit* a = arg->isa<Id>() ? arg->cast<Id>()->decl()->e()->cast<ArrayLit>() : arg->cast<ArrayLit>();
    IntArgs ia(a->v().size()+offset);
    for (int i=offset; i--;)
        ia[i] = 0;
    for (int i=a->v().size(); i--;) {
        ia[i+offset] = a->v()[i]->cast<IntLit>()->v().toInt();
    }
    return ia;
  }
  
  Gecode::IntArgs 
  GecodeSolverInstance::arg2boolargs(Expression* arg, int offset) {
    if(!arg->isa<Id>() && !arg->isa<ArrayLit>()) {
      std::stringstream ssm; ssm << "Invalid argument in arg2boolargs: " << *arg;
      ssm << ". Expected Id or ArrayLit.";
      throw InternalError(ssm.str());
    }
    ArrayLit* a = arg->isa<Id>() ? arg->cast<Id>()->decl()->e()->cast<ArrayLit>() : arg->cast<ArrayLit>();
    IntArgs ia(a->v().size()+offset);
    for (int i=offset; i--;)
        ia[i] = 0;
    for (int i=a->v().size(); i--;)
        ia[i+offset] = a->v()[i]->cast<BoolLit>()->v();
    return ia;
  }
  
  Gecode::IntSet 
  GecodeSolverInstance::arg2intset(Expression* arg) {
    SetLit* sl = NULL;
    if(Id* id = arg->dyn_cast<Id>()) {
        sl = id->decl()->e()->cast<SetLit>();
    } else if(SetLit* s = arg->dyn_cast<SetLit>()) {
        sl = s;
    } else if(BinOp* b = arg->dyn_cast<BinOp>()) {
        sl = new SetLit(arg->loc(), IntSetVal::a(getNumber<long long int>(b->lhs()),
                                                  getNumber<long long int>(b->rhs())));
    } else {
        std::stringstream ssm; ssm << "Invalid argument in arg2intset: " << *arg;
        ssm << ". Expected Id, SetLit or BinOp.";
        throw new InternalError(ssm.str());
    }
    IntSet d;
    ASTExprVec<Expression> v = sl->v();
    Region re(*this->_current_space);
    if(v.size() > 0) {
        int* is = re.alloc<int>(static_cast<unsigned long int>(v.size()));
        for(int i=0; i<v.size(); i++)
            is[i] = v[i]->cast<IntLit>()->v().toInt();
        d = IntSet(is, v.size());
    } else {
        int card = sl->isv()->card().toInt();
        int* is = re.alloc<int>(static_cast<unsigned long int>(card));
        int idx =0;
        IntSetRanges isv = IntSetRanges(sl->isv());
        for(; isv();++isv) {
            for(int i=isv.min().toInt(); i<=isv.max().toInt();i++, idx++)
                is[idx] = i;
        }
        d = IntSet(is, card);
    }
    return d;
   }
  
  Gecode::IntVarArgs 
  GecodeSolverInstance::arg2intvarargs(Expression* arg, int offset) {
    ArrayLit* a = arg2arraylit(arg);
    if (a->v().size() == 0) {
        IntVarArgs emptyIa(0);
        return emptyIa;
    }
    IntVarArgs ia(a->v().size()+offset);
    for (int i=offset; i--;)
        ia[i] = IntVar(*this->_current_space, 0, 0);
    for (int i=a->v().size(); i--;) {
        Expression* e = a->v()[i];
        int idx;
        if (e->type().isvar()) {
            //ia[i+offset] = _current_space->iv[*(int*)resolveVar(getVarDecl(e))];            
            GecodeSolver::Variable var = resolveVar(getVarDecl(e));
            assert(var.isint());
            Gecode::IntVar v = var.intVar();
            ia[i+offset] = v;            
        } else {
            int value = e->cast<IntLit>()->v().toInt();
            IntVar iv(*this->_current_space, value, value);
            ia[i+offset] = iv;
        }
    }
    return ia;
  }
  
  Gecode::BoolVarArgs 
  GecodeSolverInstance::arg2boolvarargs(Expression* arg, int offset, int siv) {
    ArrayLit* a = arg2arraylit(arg);
    if (a->length() == 0) {
        BoolVarArgs emptyIa(0);
        return emptyIa;
    }
    BoolVarArgs ia(a->length()+offset-(siv==-1?0:1));
    for (int i=offset; i--;)
        ia[i] = BoolVar(*this->_current_space, 0, 0);
    for (int i=0; i<static_cast<int>(a->length()); i++) {
        if (i==siv)
            continue;
        Expression* e = a->v()[i];
        if(e->type().isvar()) {
            GecodeVariable var = resolveVar(getVarDecl(e));
            if (e->type().isvarbool()) {
              assert(var.isbool());
              ia[offset++] = var.boolVar();
            } else if(e->type().isvarint() && var.hasBoolAlias()) {
              ia[offset++] = _current_space->bv[var.boolAliasIndex()];
            }            
            else {
              std::stringstream ssm; 
              ssm << "expected bool-var or alias int var instead of " << *e 
                  << " with type " << e->type().toString() ;
              throw InternalError(ssm.str());             
            }
        } else {
          if(BoolLit* bl = e->dyn_cast<BoolLit>()) {
            bool value = bl->v();
            BoolVar iv(*this->_current_space, value, value);
            ia[offset++] = iv;
          } else {
            std::stringstream ssm; ssm << "Expected bool literal instead of: " << *e;            
            throw new InternalError(ssm.str());
          }
        }
    }
    return ia;
  }
  
  Gecode::BoolVar 
  GecodeSolverInstance::arg2boolvar(Expression* e) {
    BoolVar x0;
    if (e->type().isvar()) {
        //x0 = _current_space->bv[*(int*)resolveVar(getVarDecl(e))];
        GecodeVariable var = resolveVar(getVarDecl(e));
        assert(var.isbool());
        x0 = var.boolVar();
    } else {
      if(BoolLit* bl = e->dyn_cast<BoolLit>()) {
        x0 = BoolVar(*this->_current_space, bl->v(), bl->v());
      } else {
        std::stringstream ssm; ssm << "Expected bool literal instead of: " << *e;            
        throw new InternalError(ssm.str());
      }
    }
    return x0;
  }
  
  Gecode::IntVar 
  GecodeSolverInstance::arg2intvar(Expression* e) {
    IntVar x0;
    if (e->type().isvar()) {
        //x0 = _current_space->iv[*(int*)resolveVar(getVarDecl(e))];
        GecodeVariable var = resolveVar(getVarDecl(e));
        assert(var.isint());
        x0 = var.intVar();
    } else {
        IntVal i;
        if(IntLit* il = e->dyn_cast<IntLit>()) i = il->v().toInt();
        else if(BoolLit* bl = e->dyn_cast<BoolLit>()) i = bl->v();
        else { 
          std::stringstream ssm; ssm << "Expected bool or int literal instead of: " << *e;
          throw InternalError(ssm.str());
        }
        x0 = IntVar(*this->_current_space, i.toInt(), i.toInt());
    }
    return x0;
  }
  
  ArrayLit* 
  GecodeSolverInstance::arg2arraylit(Expression* arg) {
    ArrayLit* a;
      if(Id* id = arg->dyn_cast<Id>()) {
          VarDecl* vd = id->decl();
          if(vd->e()) {
              a = vd->e()->cast<ArrayLit>();
          } else {
              std::vector<Expression*>* array = arrayMap[vd];
              std::vector<Expression*> ids;
              for(unsigned int i=0; i<array->size(); i++)
                  ids.push_back(((*array)[i])->cast<VarDecl>()->id());
              a = new ArrayLit(vd->loc(), ids);
          }
      } else if(ArrayLit* al = arg->dyn_cast<ArrayLit>()) {
          a = al;
      } else {
          std::stringstream ssm; ssm << "Invalid argument in arg2arrayLit: " << *arg;
          ssm << ". Expected Id or ArrayLit."; 
          throw new InternalError(ssm.str());
      }
      return a; 
  }
  
  bool 
  GecodeSolverInstance::isBoolArray(ArrayLit* a, int& singleInt) {    
    singleInt = -1;
    if (a->length() == 0)
        return true;
    for (int i=a->length(); i--;) {
        if (a->v()[i]->type().isbool()) {
          continue;
        } else if ((a->v()[i])->type().isvarint()) {
          GecodeVariable var = resolveVar(getVarDecl(a->v()[i]));
          if (var.hasBoolAlias()) {            
            if (singleInt != -1) {
              return false;
            }
            singleInt = var.boolAliasIndex();
          }
          else return false;
        } else {
          return false;
        }
    }
    return singleInt==-1 || a->length() > 1;    
  }
  
#ifdef GECODE_HAS_FLOAT_VARS
  Gecode::FloatValArgs 
  GecodeSolverInstance::arg2floatargs(Expression* arg, int offset) {
    assert(!arg->isa<Id>() && !arg->isa<ArrayLit>());
    ArrayLit* a = arg->isa<Id>() ? arg->cast<Id>()->decl()->e()->cast<ArrayLit>() : arg->cast<ArrayLit>();
    FloatValArgs fa(a->v().size()+offset);
    for (int i=offset; i--;)
        fa[i] = 0.0;
    for (int i=a->v().size(); i--;)
        fa[i+offset] = a->v()[i]->cast<FloatLit>()->v();
    return fa;
  }
  
  Gecode::FloatVar 
  GecodeSolverInstance::arg2floatvar(Expression* e) {
    FloatVar x0;
    if (e->type().isvar()) {      
      GecodeVariable var = resolveVar(getVarDecl(e));
      assert(var.isfloat());
      x0 = var.floatVar();        
    } else {
        FloatVal i;
        if(IntLit* il = e->dyn_cast<IntLit>()) i = il->v().toInt();
        else if(BoolLit* bl = e->dyn_cast<BoolLit>()) i = bl->v();
        else if(FloatLit* fl = e->dyn_cast<FloatLit>()) i = fl->v();
        else {
          std::stringstream ssm; ssm << "Expected bool, int or float literal instead of: " << *e;
          throw InternalError(ssm.str());
        }
        x0 = FloatVar(*this->_current_space, i, i);
    }
    return x0;
  }
  
  Gecode::FloatVarArgs 
  GecodeSolverInstance::arg2floatvarargs(Expression* arg, int offset) {
    ArrayLit* a = arg2arraylit(arg);
    if (a->v().size() == 0) {
        FloatVarArgs emptyFa(0);
        return emptyFa;
    }
    FloatVarArgs fa(a->v().size()+offset);
    for (int i=offset; i--;)
        fa[i] = FloatVar(*this->_current_space, 0.0, 0.0);
    for (int i=a->v().size(); i--;) {
        Expression* e = a->v()[i];
        if (e->type().isvar()) {            
            GecodeVariable var = resolveVar(getVarDecl(e));
            assert(var.isfloat());
            fa[i+offset] = var.floatVar();
        } else {
          if(FloatLit* fl = e->dyn_cast<FloatLit>()) {
            double value = fl->v();
            FloatVar fv(*this->_current_space, value, value);
            fa[i+offset] = fv;
          } else {
            std::stringstream ssm; ssm << "Expected float literal instead of: " << *e;
            throw InternalError(ssm.str());
          }           
        }
    }
    return fa;
  }
#endif

  Gecode::IntConLevel 
  GecodeSolverInstance::ann2icl(const Annotation& ann) {
    if (!ann.isEmpty()) {
      if (getAnnotation(ann, "val"))
          return Gecode::ICL_VAL;
      if (getAnnotation(ann, "domain"))
          return Gecode::ICL_DOM;
      if (getAnnotation(ann, "bounds") ||
              getAnnotation(ann, "boundsR") ||
              getAnnotation(ann, "boundsD") ||
              getAnnotation(ann, "boundsZ"))
          return Gecode::ICL_BND;
    }
    return Gecode::ICL_DEF;
  }
  
  VarDecl* 
  GecodeSolverInstance::getVarDecl(Expression* expr) {
    VarDecl* vd=NULL;
    if( (vd = expr->dyn_cast<VarDecl>()) ) {
        vd = expr->cast<VarDecl>();
    } else if(Id* id = expr->dyn_cast<Id>()) {
        vd = id->decl();
    } else if(ArrayAccess* aa = expr->dyn_cast<ArrayAccess>()) {
        vd = resolveArrayAccess(aa);
    } else {
        std::stringstream ssm; ssm << "Can not extract vardecl from " << *expr; 
        throw new InternalError(ssm.str());
    }
    return vd;
  }
  
  VarDecl* 
  GecodeSolverInstance::resolveArrayAccess(ArrayAccess* aa) {
    VarDecl* vd = aa->v()->cast<Id>()->decl();
    int idx = aa->idx()[0]->cast<IntLit>()->v().toInt();
    return resolveArrayAccess(vd, idx);
  }
  
  VarDecl* 
  GecodeSolverInstance::resolveArrayAccess(VarDecl* vd, int index) {
    UNORDERED_NAMESPACE::unordered_map<VarDecl*, std::vector<Expression*>* >::iterator it = arrayMap.find(vd);
    if(it != arrayMap.end()) {
        std::vector<Expression*>* exprs = it->second;
        Expression* expr = (*exprs)[index-1];
        return expr->cast<VarDecl>();
    } else {
        std::stringstream ssm; ssm << "Unknown array: " << vd->id();
        throw new InternalError(ssm.str());
    }
  }
  
  GecodeSolver::Variable 
  GecodeSolverInstance::resolveVar(Expression* e) {
    if (Id* id = e->dyn_cast<Id>()) {
        return _variableMap.get(id); //lookupVar(id->decl());
    } else if (VarDecl* vd = e->dyn_cast<VarDecl>()) {
        return _variableMap.get(vd->id()->decl()->id());
    } else if (ArrayAccess* aa = e->dyn_cast<ArrayAccess>()) {
        return _variableMap.get(resolveArrayAccess(aa)->id());
    } else {
        std::stringstream ssm; 
        ssm << "Expected Id, VarDecl or ArrayAccess instead of \"" << *e << "\"";
        throw InternalError(ssm.str());
    }
  }
  
  SolverInstance::Status 
  GecodeSolverInstance::next(void) {
    assert(false); // TODO: implement
  }
  
  void 
  GecodeSolverInstance::resetSolver(void) {
    assert(false); // TODO: implement
  }
  
  Expression* 
  GecodeSolverInstance::getSolutionValue(Id* id) {
    assert(false); // TODO: implement
  }
  
  SolverInstanceBase::Status 
  GecodeSolverInstance::solve(void) {
   return SolverInstanceBase::Status::ERROR; // TODO: implement   
  }
  
  FznSpace::FznSpace(bool share, FznSpace& f) : Space(share, f) {
    // integer variables
    iv.resize(f.iv.size());
    for(int i=0; i<iv.size(); i++) 
      iv[i].update(*this, share, f.iv[i]);
    for(int i=0; i<f.iv_introduced.size(); i++) 
      iv_introduced.push_back(f.iv_introduced[i]);
    for(int i=0; i<f.iv_defined.size(); i++) 
      iv_defined.push_back(f.iv_defined[i]);
    if(f._copyAuxVars) {
      IntVarArgs iva;
      for (int i=0; i<f.iv_aux.size(); i++) {
        if (!f.iv_aux[i].assigned()) {
          iva << IntVar();
          iva[iva.size()-1].update(*this, share, f.iv_aux[i]);
        }
      }
      iv_aux = IntVarArray(*this, iva);
    }    
    
    // boolean variables    
    bv.resize(f.bv.size());
    for(int i=0; i<bv.size(); i++) 
    bv[i].update(*this, share, f.bv[i]);
    if (f._copyAuxVars) {
      BoolVarArgs bva;
      for (int i=0; i<f.bv_aux.size(); i++) {
        if (!f.bv_aux[i].assigned()) {
          bva << BoolVar();
          bva[bva.size()-1].update(*this, share, f.bv_aux[i]);
        }
      }
      bv_aux = BoolVarArray(*this, bva);
    }
    for(int i=0; i<f.bv_introduced.size(); i++) 
      bv_introduced.push_back(f.bv_introduced[i]);
    
    
#ifdef GECODE_HAS_SET_VARS
    sv.resize(f.sv.size());
    for(int i=0; i<sv.size(); i++)
      sv[i].update(*this, share, f.sv[i]);  
    if (f._copyAuxVars) {
      SetVarArgs sva;
      for (int i=0; i<f.sv_aux.size(); i++) {
        if (!f.sv_aux[i].assigned()) {
          sva << SetVar();
          sva[sva.size()-1].update(*this, share, f.sv_aux[i]);
        }
      }
      sv_aux = SetVarArray(*this, sva);
    }
    for(int i=0; i<f.sv_introduced.size(); i++) 
      sv_introduced.push_back(f.sv_introduced[i]);            
#endif
      
#ifdef GECODE_HAS_FLOAT_VARS
    fv.resize(f.fv.size());
    for(int i=0; i<fv.size(); i++)
      fv[i].update(*this, share, f.fv[i]);
    if (f._copyAuxVars) {
      FloatVarArgs fva;
      for (int i=0; i<f.fv_aux.size(); i++) {
        if (!f.fv_aux[i].assigned()) {
          fva << FloatVar();
          fva[fva.size()-1].update(*this, share, f.fv_aux[i]);
        }
      }
      fv_aux = FloatVarArray(*this, fva);
    }    
#endif
  }
  
  void 
  FznSpace::constrain(const Space& s) {
    if (_optVarIsInt) {
            if (_solveType == MiniZinc::SolveI::SolveType::ST_MIN)
                rel(*this, iv[_optVarIdx], IRT_LE,
                    static_cast<const FznSpace*>(&s)->iv[_optVarIdx].val());
            else if (_solveType == MiniZinc::SolveI::SolveType::ST_MAX)
                rel(*this, iv[_optVarIdx], IRT_GR,
                    static_cast<const FznSpace*>(&s)->iv[_optVarIdx].val());
        } else {
#ifdef GECODE_HAS_FLOAT_VARS
            if (_solveType == MiniZinc::SolveI::SolveType::ST_MIN)
                rel(*this, fv[_optVarIdx], FRT_LE,
                    static_cast<const FznSpace*>(&s)->fv[_optVarIdx].val());
            else if (_solveType == MiniZinc::SolveI::SolveType::ST_MAX)
                rel(*this, fv[_optVarIdx], FRT_GR,
                    static_cast<const FznSpace*>(&s)->fv[_optVarIdx].val());
#endif
        }   
  }
  
  Gecode::Space* 
  FznSpace::copy(bool share) {
   return new FznSpace(share, *this);
  }
 
}