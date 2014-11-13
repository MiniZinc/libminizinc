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
#include "aux_brancher.hh"
#include "fzn_space.hh"

using namespace Gecode;

namespace MiniZinc {
  
     GecodeSolverInstance::GecodeSolverInstance(Env& env, const Options& options) 
     : SolverInstanceImpl<GecodeSolver>(env,options), _current_space(NULL), _solution(NULL) {
       registerConstraints(); 
       // processFlatZinc(); // TODO: shouldn't this better be in the constructor?
     }
  
    GecodeSolverInstance::~GecodeSolverInstance(void) {
      //delete _current_space;
      // delete _solution; // TODO: is this necessary?
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
        // check if it has an output-annotation
        VarDecl* vd = it->e();
        if(!vd->ann().isEmpty()) {
          if(vd->ann().containsCall(constants().ann.output_array.aststr()) || 
            vd->ann().containsCall(constants().ann.output_var->str())
          ) {            
            _varsWithOutput.push_back(vd);
          }
        }
        
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
                                            GecodeVariable(GecodeVariable::INT_TYPE, 
                                                           _current_space->iv.size()-1));
                    } else {                                      
                        std::pair<double,double> bounds = getIntBounds(domain); 
                        int lb = bounds.first;
                        int ub = bounds.second;  
                        IntVar intVar(*this->_current_space, lb, ub);
                        _current_space->iv.push_back(intVar);    
                        _variableMap.insert(it->e()->id(), GecodeVariable(GecodeVariable::INT_TYPE, 
                                                           _current_space->iv.size()-1));
                    }
                } else {
                    int lb = Gecode::Int::Limits::min;
                    int ub = Gecode::Int::Limits::max;
                    IntVar intVar(*this->_current_space, lb, ub);
                    _current_space->iv.push_back(intVar);
                    _variableMap.insert(it->e()->id(), GecodeVariable(GecodeVariable::INT_TYPE, 
                                                           _current_space->iv.size()-1));
                }
            } else { // there is an initialisation expression
                Expression* init = it->e()->e();                
                if (init->isa<Id>() || init->isa<ArrayAccess>()) {
                   // root->iv[root->intVarCount++] = root->iv[*(int*)resolveVar(init)];                                      
                   GecodeVariable var = resolveVar(init);
                   assert(var.isint());
                  _current_space->iv.push_back(var.intVar(_current_space));
                  _variableMap.insert(it->e()->id(), var);                                  
                } else {
                    double il = init->cast<IntLit>()->v().toInt();
                    IntVar intVar(*this->_current_space, il, il);
                    _current_space->iv.push_back(intVar);
                    _variableMap.insert(it->e()->id(), GecodeVariable(GecodeVariable::INT_TYPE, 
                                                           _current_space->iv.size()-1));
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
                _variableMap.insert(it->e()->id(), GecodeVariable(GecodeVariable::BOOL_TYPE, 
                                                           _current_space->bv.size()-1));
            } else { // there is an initialisation expression
                Expression* init = it->e()->e();
                if (init->isa<Id>() || init->isa<ArrayAccess>()) {
                    // root->bv[root->boolVarCount++] = root->bv[*(int*)resolveVar(init)];                  
                    //int index = *(int*) resolveVar(init);
                    GecodeVariable var = resolveVar(init);
                    assert(var.isbool());                    
                    _current_space->bv.push_back(var.boolVar(_current_space));
                    _variableMap.insert(it->e()->id(), var);                                    
                } else {
                    double b = (double) init->cast<BoolLit>()->v();
                    BoolVar boolVar(*this->_current_space, b, b);
                    _current_space->bv.push_back(boolVar);
                    _variableMap.insert(it->e()->id(), GecodeVariable(GecodeVariable::BOOL_TYPE, 
                                                           _current_space->bv.size()-1));
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
                _variableMap.insert(it->e()->id(), GecodeVariable(GecodeVariable::FLOAT_TYPE, 
                                                           _current_space->fv.size()-1));
            } else {
                Expression* init = it->e()->e();
                if (init->isa<Id>() || init->isa<ArrayAccess>()) {
                    // root->fv[root->floatVarCount++] = root->fv[*(int*)resolveVar(init)];                                      
                    GecodeVariable var = resolveVar(init);
                    assert(var.isfloat());                   
                    _current_space->fv.push_back(var.floatVar(_current_space));
                    _variableMap.insert(it->e()->id(), var);                          
                } else {
                    double il = init->cast<FloatLit>()->v();
                    FloatVar floatVar(*this->_current_space, il, il);
                    _current_space->fv.push_back(floatVar);
                    _variableMap.insert(it->e()->id(), GecodeVariable(GecodeVariable::FLOAT_TYPE, 
                                                           _current_space->fv.size()-1));
                }
            }
            isIntroduced = it->e()->introduced() || (MiniZinc::getAnnotation(it->e()->ann(), constants().ann.is_introduced.str()) != NULL);
            _current_space->fv_introduced.push_back(isIntroduced);
            isDefined = MiniZinc::getAnnotation(it->e()->ann(), constants().ann.is_defined_var->str().str()) != NULL;
            _current_space->fv_defined.push_back(isDefined);            
          }
          break;                     
          // TODO: SetVars
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
          IntVar intVar = var.intVar(_current_space);
          for(int i=0; i<_current_space->iv.size(); i++) {
            if(_current_space->iv[i].same(intVar)) {
              _current_space->_optVarIdx = i;
              break;
            }
          }
          assert(_current_space->_optVarIdx >= 0);
        } else {
          FloatVar floatVar = var.floatVar(_current_space);
          for(int i=0; i<_current_space->fv.size(); i++) {
            if(_current_space->fv[i].same(floatVar)) {
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
            Gecode::IntVar v = var.intVar(_current_space);
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
              ia[offset++] = var.boolVar(_current_space);
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
        x0 = var.boolVar(_current_space);
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
        x0 = var.intVar(_current_space);
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
      x0 = var.floatVar(_current_space);        
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
            fa[i+offset] = var.floatVar(_current_space);
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
    std::cout << "DEBUG: getting solution value of id: " << *id;
    GecodeVariable var = resolveVar(id);
    switch (id->type().bt()) {
      case Type::BT_INT: 
        assert(var.intVar(_solution).assigned());
        return new IntLit(Location(), var.intVar(_solution).val());
      case Type::BT_BOOL: 
        assert(var.boolVar(_solution).assigned());
        return new BoolLit(Location(), var.boolVar(_solution).val());
      case Type::BT_FLOAT: 
        assert(var.floatVar(_solution).assigned());
        return new FloatLit(Location(), (var.floatVar(_solution).val()).med());
      default: return NULL;          
    }    
  }
  
  SolverInstanceBase::Status 
  GecodeSolverInstance::solve(void) {
    // TODO: check what we need to do options-wise
    std::vector<Expression*> branch_vars;
    std::vector<Expression*> solve_args;
    Expression* solveExpr = _env.flat()->solveItem()->e();
    Expression* optSearch = NULL;
    
    // collect all var decls, mapping: ozn element ---> (VarDecl*, Expression)
    // initializing Expression with NULL or respective the assignment expression
    /*Model* _ozn = _env.output();
    for (unsigned int i=0; i<_ozn->size(); i++) {
      if (VarDeclI* vdi = (*_ozn)[i]->dyn_cast<VarDeclI>()) {
        _declmap.insert(std::make_pair(vdi->e()->id()->v(),DE(vdi->e(),vdi->e()->e())));
        std::cout << "DEBUG: Mapping \"" << vdi->e()->id()->v()  << "\" ---> (" << *vdi->e() << ", " << *vdi->e()->e() << ")" <<  std::endl;
      }
    } // TODO continue
    */
    
    switch(_current_space->_solveType) {
      case MiniZinc::SolveI::SolveType::ST_MIN:      
        assert(solveExpr != NULL);
        branch_vars.push_back(solveExpr);
        solve_args.push_back(new ArrayLit(Location(), branch_vars));
        if (!_current_space->_optVarIsInt) // TODO: why??
          solve_args.push_back(new FloatLit(Location(), 0.0));
        solve_args.push_back(new Id(Location(), "input_order", NULL));
        solve_args.push_back(new Id(Location(), _current_space->_optVarIsInt ? "indomain_min" : "indomain_split", NULL));
        solve_args.push_back(new Id(Location(), "complete", NULL));
        optSearch = new Call(Location(), _current_space->_optVarIsInt ? "int_search" : "float_search", solve_args);
        break;      
      case MiniZinc::SolveI::SolveType::ST_MAX:
        branch_vars.push_back(solveExpr);
        solve_args.push_back(new ArrayLit(Location(), branch_vars));
        if (!_current_space->_optVarIsInt)
          solve_args.push_back(new FloatLit(Location(), 0.0));
        solve_args.push_back(new Id(Location(), "input_order", NULL));
        solve_args.push_back(new Id(Location(), _current_space->_optVarIsInt ? "indomain_max" : "indomain_split_reverse", NULL));
        solve_args.push_back(new Id(Location(), "complete", NULL));
        optSearch = new Call(Location(), _current_space->_optVarIsInt ? "int_search" : "float_search", solve_args);
        break;
      case MiniZinc::SolveI::SolveType::ST_SAT:        
        break;
      default:
        assert(false);
    }    
    createBranchers(_env.flat()->solveItem()->ann(), optSearch, 
                    111 /* _options.getFloatParam("seed")  */, // TODO: implement
                    0.5 /* _options.getFloatParam("decay") */, // TODO: implement
                    false, /* ignoreUnknown */
                    std::cerr); 
    
    // TODO: add presolving part
            
    SolverInstanceBase::Status status;
    if(_current_space->_solveType == MiniZinc::SolveI::SolveType::ST_SAT) {
      status = runEngine<DFS>();
    }
    else {
      status = runEngine<BAB>();
      // TODO: reset the declmap in each iteration
    }               
    return status;
  }
  
  void 
  GecodeSolverInstance::createBranchers(Annotation& ann, Expression* additionalAnn, 
                                        int seed, double decay, bool ignoreUnknown, 
                                        std::ostream& err) {
    // default search heuristics
    Rnd rnd(static_cast<unsigned int>(seed));
    TieBreak<IntVarBranch> def_int_varsel = INT_VAR_AFC_SIZE_MAX(0.99);
    IntValBranch def_int_valsel = INT_VAL_MIN();
    TieBreak<IntVarBranch> def_bool_varsel = INT_VAR_AFC_MAX(0.99);
    IntValBranch def_bool_valsel = INT_VAL_MIN();
#ifdef GECODE_HAS_SET_VARS
    SetVarBranch def_set_varsel = SET_VAR_AFC_SIZE_MAX(0.99);
    SetValBranch def_set_valsel = SET_VAL_MIN_INC();
#endif
#ifdef GECODE_HAS_FLOAT_VARS
    TieBreak<FloatVarBranch> def_float_varsel = FLOAT_VAR_SIZE_MIN();
    FloatValBranch def_float_valsel = FLOAT_VAL_SPLIT_MIN();
#endif
    
    std::vector<bool> iv_searched(_current_space->iv.size());
    for (unsigned int i=_current_space->iv.size(); i--;)
      iv_searched[i] = false;
    std::vector<bool> bv_searched(_current_space->bv.size());
    for (unsigned int i=_current_space->bv.size(); i--;)
      bv_searched[i] = false;
#ifdef GECODE_HAS_SET_VARS
    std::vector<bool> sv_searched(_current_space->sv.size());
    for (unsigned int i=_current_space->sv.size(); i--;)
      sv_searched[i] = false;
#endif
#ifdef GECODE_HAS_FLOAT_VARS
    std::vector<bool> fv_searched(_current_space->fv.size());
    for (unsigned int i=_current_space->fv.size(); i--;)
      fv_searched[i] = false;
#endif
    
    // solving annotations 
    std::vector<Expression*> flatAnn;
    if (!ann.isEmpty()) {
      // flattenAnnotations(ann, flatAnn); // TODO: implement
    }
    if (additionalAnn != NULL) {
      flatAnn.push_back(additionalAnn);
    }    
    if (flatAnn.size() > 0) {
      // TODO: implement
      std::cout << "Ignoring solving annotations for now..." << std::endl;
    }
    
    int introduced = 0;
    int funcdep = 0;
    int searched = 0;
    for (int i=_current_space->iv.size(); i--;) {
      if (iv_searched[i]) {        
        searched++;         
      } else if (_current_space->iv_introduced[i]) {                  
          if (_current_space->iv_defined[i]) {
            funcdep++;
          } else {
            introduced++;
          }             
      }
    }       
    IntVarArgs iv_sol(_current_space->iv.size()-(introduced+funcdep+searched));
    IntVarArgs iv_tmp(introduced);
    for (int i=_current_space->iv.size(), j=0, k=0; i--;) {      
      if (iv_searched[i])
        continue;           
      if(_current_space->iv_introduced[i]) {                
        if(_current_space->iv_introduced.size() >= i) {
          if (!_current_space->iv_defined[i]) {                 
            iv_tmp[j++] = _current_space->iv[i];
          }                     
        }               
      } else {
          iv_sol[k++] = _current_space->iv[i];
      }
    }
    // Collecting Boolean variables
    introduced = 0;
    funcdep = 0;
    searched = 0;
    for (int i=_current_space->bv.size(); i--;) {
      if (bv_searched[i]) {
        searched++;
      } else if (_current_space->bv_introduced[i]) {
        if (_current_space->bv_defined[i]) {
          funcdep++;
        } else {
            introduced++;
        }               
      }
    }        
    BoolVarArgs bv_sol(_current_space->bv.size()-(introduced+funcdep+searched));
    BoolVarArgs bv_tmp(introduced);
    for (int i=_current_space->bv.size(), j=0, k=0; i--;) {
      if (bv_searched[i])
        continue;
      if (_current_space->bv_introduced[i]) {
        if (!_current_space->bv_defined[i]) {
            bv_tmp[j++] = _current_space->bv[i];
        }
      } else {
          bv_sol[k++] = _current_space->bv[i];
      }
    }      
    
    if (iv_sol.size() > 0)
      branch(*this->_current_space, iv_sol, def_int_varsel, def_int_valsel);
    if (bv_sol.size() > 0)
      branch(*this->_current_space, bv_sol, def_bool_varsel, def_bool_valsel);
    
    std::cout << "DEBUG: branched over " << iv_sol.size()  << " integer variables."<< std::endl;
    std::cout << "DEBUG: branched over " << bv_sol.size()  << " Boolean variables."<< std::endl;
#ifdef GECODE_HAS_FLOAT_VARS
    introduced = 0;
    funcdep = 0;
    searched = 0;
    for (int i=_current_space->fv.size(); i--;) {
      if (fv_searched[i]) {
        searched++;
      } else if (_current_space->fv_introduced[i]) {
        if (_current_space->fv_defined[i]) {
          funcdep++;
        } else {
          introduced++;
        }
      }
    }
    FloatVarArgs fv_sol(_current_space->fv.size()-(introduced+funcdep+searched));
    FloatVarArgs fv_tmp(introduced);
    for (int i=_current_space->fv.size(), j=0, k=0; i--;) {
      if (fv_searched[i])
        continue;
      if (_current_space->fv_introduced[i]) {
        if (!_current_space->fv_defined[i]) {
          fv_tmp[j++] = _current_space->fv[i];
        }
      } else {
        fv_sol[k++] = _current_space->fv[i];
      }
    }

    if (fv_sol.size() > 0)
      branch(*this->_current_space, fv_sol, def_float_varsel, def_float_valsel);
#endif
#ifdef GECODE_HAS_SET_VARS
    introduced = 0;
    funcdep = 0;
    searched = 0;
    for (int i=_current_space->sv.size(); i--;) {
      if (sv_searched[i]) {
          searched++;
      } else if (_current_space->sv_introduced[i]) {
          if (_current_space->sv_defined[i]) {
              funcdep++;
          } else {
              introduced++;
          }
      }
    }
    SetVarArgs sv_sol(_current_space->sv.size()-(introduced+funcdep+searched));
    SetVarArgs sv_tmp(introduced);
    for (int i=_current_space->sv.size(), j=0, k=0; i--;) {
      if (sv_searched[i])
          continue;
      if (_current_space->sv_introduced[i]) {
          if (!_current_space->sv_defined[i]) {
              sv_tmp[j++] = _current_space->sv[i];
          }
      } else {
          sv_sol[k++] = _current_space->sv[i];
      }
    }

    if (sv_sol.size() > 0)
      branch(*this->_current_space, sv_sol, def_set_varsel, def_set_valsel);
#endif
      
    // branching on auxiliary variables
    _current_space->iv_aux = IntVarArray(*this->_current_space, iv_tmp);
    _current_space->bv_aux = BoolVarArray(*this->_current_space, bv_tmp);
    int n_aux = _current_space->iv_aux.size() + _current_space->bv_aux.size();
#ifdef GECODE_HAS_SET_VARS
    _current_space->sv_aux = SetVarArray(*this->_current_space, sv_tmp);
    n_aux += _current_space->sv_aux.size();
#endif
#ifdef GECODE_HAS_FLOAT_VARS
    _current_space->fv_aux = FloatVarArray(*this->_current_space, fv_tmp);
    n_aux += _current_space->fv_aux.size();
#endif
    if (n_aux > 0) {      
      AuxVarBrancher::post(*this->_current_space, def_int_varsel, def_int_valsel,
                          def_bool_varsel, def_bool_valsel
#ifdef GECODE_HAS_SET_VARS
                        , def_set_varsel, def_set_valsel
#endif
#ifdef GECODE_HAS_FLOAT_VARS
                        , def_float_varsel, def_float_valsel
#endif
                    ); // end post                    
      std::cout << "DEBUG: Posted aux-var-brancher for " << n_aux << " aux-variables" << std::endl;
    } // end if n_aux > 0 
    else 
      std::cout << "DEBUG: No aux vars to branch on." << std::endl;
  }
  
    
  template<template<class> class Engine>
    SolverInstanceBase::Status GecodeSolverInstance::runEngine() {
    if (true) {//_options.getBoolParam(ASTString("restarts"))) { // TODO: implement option
      return runMeta<Engine,Driver::EngineToMeta>();
    } else {
      return runMeta<Engine,RBS>();
    }     
   }
      
  template<template<class> class Engine,
    template<template<class> class,class> class Meta>
        SolverInstanceBase::Status GecodeSolverInstance::runMeta() {
    Search::Options o;
    o.stop = Driver::CombinedStop::create(100000, //_options.getIntParam(ASTString("nodes")), // TODO: implement option
                                          100000, //_options.getIntParam(ASTString("fails")), // TODO: implement option
                                          (unsigned int) (1000 //_options.getFloatParam(ASTString("time")) 
                                          * 1000), // TODO: implement option
                                          true);
    // TODO: other options (see below)
    //o.c_d = opts->c_d();
    //o.a_d = opts->a_d();
    //o.threads = opts->threads();
    //o.nogoods_limit = opts->nogoods() ? opts->nogoods_limit() : 0;
    //o.cutoff  = Driver::createCutoff(*opts);
    //if (opts->interrupt())
    //    Driver::CombinedStop::installCtrlHandler(true);
    Meta<Engine,FznSpace> se(this->_current_space,o);
        
    while (FznSpace* next_sol = se.next()) {
      if(_solution) delete _solution;
      _solution = next_sol;      
    }
    
    SolverInstance::Status status = SolverInstance::ERROR;
    if (!se.stopped()) {
      if(_solution) {
        if(_env.flat()->solveItem()->st() == SolveI::SolveType::ST_SAT) {
          status = SolverInstance::SAT;
          for(int i=0; i<_solution->iv.size(); i++) {
            IntVar iv = _solution->iv[i];
            if(iv.assigned())
              std::cout << iv << " = " << iv.val() << std::endl;
          }
          assignSolutionToOutput();
        } else 
          status = SolverInstance::OPT;
          assignSolutionToOutput();
      } else {
        status = SolverInstance::UNSAT;
      }
    } else {         
      if(_solution) 
         assignSolutionToOutput();
        // TODO: is that correct? what if(_solution)??
      status = SolverInstance::UNKNOWN;            
    }
    return status;
  }
 
 
  void
  GecodeSolverInstance::assignSolutionToOutput(void) {
    
    //TODO: iterate over set of ids that have an output annotation and obtain their right hand side from the flat model
    for(unsigned int i=0; i<_varsWithOutput.size(); i++) {
      VarDecl* vd = _varsWithOutput[i];
      std::cout << "DEBUG: Looking at var-decl with output-annotation: " << *vd << std::endl;
      if(vd->ann().containsCall(constants().ann.output_array.aststr())) {       
        /*for(ExpressionSetIter it = vd->ann().begin(); it != vd->ann().end(); ++it) {
          if(Call* call = (*it)->dyn_cast<Call>()) {
            std::cout << "DEBUG: Down to call " << *call << std::endl;
            std::cout << "DEBUG: with rhs: " << *(vd->e()) << std::endl;
          }
        }*/        
        assert(vd->e());
        if(ArrayLit* al = vd->e()->dyn_cast<ArrayLit>()) {
          std::vector<Expression*> array_elems;
          std::cout << "DEBUG: Down to array lit: " << *al << std::endl;
          if(al->flat() || al->dims() == 1) {
            ASTExprVec<Expression> array = al->v();
            for(unsigned int j=0; j<array.size(); j++) {
              if(Id* id = array[j]->dyn_cast<Id>()) {
                std::cout << "DEBUG: getting solution value from " << *id << std::endl;
                array_elems.push_back(getSolutionValue(id));             
                // TODO: continue: gecode variable is not assigned -> we're probably pointing to the variable of the root space....
              } else if(IntLit* intLit = array[j]->dyn_cast<IntLit>()) {              
                // TODO
              } else if(BoolLit* intLit = array[j]->dyn_cast<BoolLit>()) { 
                // TODO
              } else {
                std::cout << "DEBUG: array element " << *array[j] << " is not an id nor a literal" << std::endl;
              }
            }
            for(unsigned int j=0; j<array_elems.size(); j++) {
                std::cout << "DEBUG: solution " << j << ": " << array_elems[j] << std::endl;
            }
          } // TODO: else
        }
      } else if(vd->ann().containsCall(constants().ann.output_var->str())) {
        
      }        
    }
    
    // TODO: Iterate over the solutions in the model and set their corresponding output value in the output model
    
    /*std::cout << "DEBUG: printing variable map:" << std::endl;
    for(UNORDERED_NAMESPACE::unordered_map<Id*,GecodeSolver::Variable,ExpressionHash,IdEq>::iterator it = _variableMap.begin(); it!=_variableMap.end(); ++it) {
      if(it->second.isint()) {
        IntVar iv = it->second.intVar();
        std::cout << "\tvariable " << *(it->first) << " --> "<< iv << std::endl;
      } else if(it->second.isbool()) {
        BoolVar bv = it->second.boolVar();
        std::cout << "\tvariable " << *(it->first) << " --> " << bv << std::endl;
      }
    }
    
    unsigned int i = 0;
    for (VarDeclIterator it = _env.output()->begin_vardecls(); it != _env.output()->end_vardecls(); ++it) {
      std::cout << "DEBUG: checking declaration " << i++ << std::endl;
      if (it->e()->e() == NULL) {
        //if(_declmap.find(it->e())) { // TODO: we need to compare to an ASTString!!
        //  std::cout << "DEBUG: _declmap contains vardecl" << std::endl;
       // }
        VarDecl* vd = it->e();
        ASTStringMap<DE>::t::iterator itd = _declmap.find(vd->id()->v());
        if (itd==_declmap.end()) {
          std::cerr << "Error: unexpected identifier " << vd->id() << " in output\n";
          exit(EXIT_FAILURE);
        } else {
          std::cout << "DEBUG: found the identifier in the declmap" << std::endl;
          itd->second.first->e(vd->e()); // TODO: include arrayX calls
        }
        vd->e(getSolutionValue(vd->id()));
      } else {
        std::cout << "DEBUG: the assignment value is not NULL of " << *it;
      }
    }
    // TODO: continue
    std::cout << "DEBUG: doing something else too ...." << std::endl;
    */
  }
 
}