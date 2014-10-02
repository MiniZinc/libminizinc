/*
 *  Main authors:
 *     Kevin Leo <kevin.leo@monash.edu>
 *     Andrea Rendl <andrea.rendl@nicta.com.au>
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MINIZINC_GECODE_CONSTRAINTS_HH__
#define __MINIZINC_GECODE_CONSTRAINTS_HH__

#include <gecode/kernel.hh>
#include <gecode/int.hh>

#include <minizinc/solver_instance_base.hh>
#include "gecode_solverinstance.hh"

namespace MiniZinc {

    namespace GecodeConstraints {
      
      inline Gecode::IntRelType swap(Gecode::IntRelType irt) {
	    switch (irt) {
		case Gecode::IRT_LQ: return Gecode::IRT_GQ;
		case Gecode::IRT_LE: return Gecode::IRT_GR;
		case Gecode::IRT_GQ: return Gecode::IRT_LQ;
		case Gecode::IRT_GR: return Gecode::IRT_LE;
		default:     return irt;
	    }
	}

      inline Gecode::IntRelType neg(Gecode::IntRelType irt) {
	  switch (irt) {
	    case Gecode::IRT_EQ: return Gecode::IRT_NQ;
	      case Gecode::IRT_NQ: return Gecode::IRT_EQ;
	      case Gecode::IRT_LQ: return Gecode::IRT_GR;
	      case Gecode::IRT_LE: return Gecode::IRT_GQ;
	      case Gecode::IRT_GQ: return Gecode::IRT_LE;
	      case Gecode::IRT_GR:
	      default:
			    assert(irt == Gecode::IRT_GR);
	  }
	  return Gecode::IRT_LQ;
      }


      void p_distinct(SolverInstanceBase& s, const Call* call);
      void p_distinctOffset(SolverInstanceBase& s, const Call* call);
      void p_all_equal(SolverInstanceBase& s, const Call* call);
      void p_int_CMP(GecodeSolverInstance& s, Gecode::IntRelType irt, const Call* ce);
      void p_int_eq(SolverInstanceBase& s, const Call* call);
      void p_int_ne(SolverInstanceBase& s, const Call* call);
      void p_int_ge(SolverInstanceBase& s, const Call* call);
      void p_int_gt(SolverInstanceBase& s, const Call* call);
      void p_int_le(SolverInstanceBase& s, const Call* call);
      void p_int_lt(SolverInstanceBase& s, const Call* call);
      void p_int_CMP_reif(GecodeSolverInstance& s, Gecode::IntRelType irt, Gecode::ReifyMode rm, const Call* call);
      ///* Comparisons */
      void p_int_eq_reif(SolverInstanceBase& s, const Call* call);
      void p_int_ne_reif(SolverInstanceBase& s, const Call* call);
      void p_int_ge_reif(SolverInstanceBase& s, const Call* call);
      void p_int_gt_reif(SolverInstanceBase& s, const Call* call);
      void p_int_le_reif(SolverInstanceBase& s, const Call* call);
      void p_int_lt_reif(SolverInstanceBase& s, const Call* call);
      void p_int_eq_imp(SolverInstanceBase& s, const Call* call);
      void p_int_ne_imp(SolverInstanceBase& s, const Call* call);
      void p_int_ge_imp(SolverInstanceBase& s, const Call* call);
      void p_int_gt_imp(SolverInstanceBase& s, const Call* call);
      void p_int_le_imp(SolverInstanceBase& s, const Call* call);
      void p_int_lt_imp(SolverInstanceBase& s, const Call* call);
      void p_int_lin_CMP(GecodeSolverInstance& s, Gecode::IntRelType irt, const Call* call) ;
      void p_int_lin_CMP_reif(GecodeSolverInstance& s, Gecode::IntRelType irt, Gecode::ReifyMode rm, const Call* call);
      void p_int_lin_eq(SolverInstanceBase& s, const Call* call);
      void p_int_lin_eq_reif(SolverInstanceBase& s, const Call* call);
      void p_int_lin_eq_imp(SolverInstanceBase& s, const Call* call);
      void p_int_lin_ne(SolverInstanceBase& s, const Call* call);
      void p_int_lin_ne_reif(SolverInstanceBase& s, const Call* call);
      void p_int_lin_ne_imp(SolverInstanceBase& s, const Call* call);
      void p_int_lin_le(SolverInstanceBase& s, const Call* call) ;
      void p_int_lin_le_reif(SolverInstanceBase& s, const Call* call);
      void p_int_lin_le_imp(SolverInstanceBase& s, const Call* call);
      void p_int_lin_lt(SolverInstanceBase& s, const Call* call);
      void p_int_lin_lt_reif(SolverInstanceBase& s, const Call* call);
      void p_int_lin_lt_imp(SolverInstanceBase& s, const Call* call);
      void p_int_lin_ge(SolverInstanceBase& s, const Call* call);
      void p_int_lin_ge_reif(SolverInstanceBase& s, const Call* call);
      void p_int_lin_ge_imp(SolverInstanceBase& s, const Call* call);
      void p_int_lin_gt(SolverInstanceBase& s, const Call* call);
      void p_int_lin_gt_reif(SolverInstanceBase& s, const Call* call);
      void p_int_lin_gt_imp(SolverInstanceBase& s, const Call* call);
      void p_bool_lin_CMP(GecodeSolverInstance& s, Gecode::IntRelType irt, const Call* call);
      void p_bool_lin_CMP_reif(GecodeSolverInstance& s, Gecode::IntRelType irt, Gecode::ReifyMode rm, const Call* call);
      void p_bool_lin_eq(SolverInstanceBase& s, const Call* call);
      void p_bool_lin_eq_reif(SolverInstanceBase& s, const Call* call);
      void p_bool_lin_eq_imp(SolverInstanceBase& s, const Call* call) ;
      void p_bool_lin_ne(SolverInstanceBase& s, const Call* call);
      void p_bool_lin_ne_reif(SolverInstanceBase& s, const Call* call) ;
      void p_bool_lin_ne_imp(SolverInstanceBase& s, const Call* call) ;
      void p_bool_lin_le(SolverInstanceBase& s, const Call* call);
      void p_bool_lin_le_reif(SolverInstanceBase& s, const Call* call) ;
      void p_bool_lin_le_imp(SolverInstanceBase& s, const Call* call) ;
      void p_bool_lin_lt(SolverInstanceBase& s, const Call* call) ;
      void p_bool_lin_lt_reif(SolverInstanceBase& s, const Call* call);
      void p_bool_lin_lt_imp(SolverInstanceBase& s, const Call* call) ;
      void p_bool_lin_ge(SolverInstanceBase& s, const Call* call);
      void p_bool_lin_ge_reif(SolverInstanceBase& s, const Call* call) ;
      void p_bool_lin_ge_imp(SolverInstanceBase& s, const Call* call) ;
      void p_bool_lin_gt(SolverInstanceBase& s, const Call* call) ;
      void p_bool_lin_gt_reif(SolverInstanceBase& s, const Call* call) ;
      void p_bool_lin_gt_imp(SolverInstanceBase& s, const Call* call) ;
      
      ///* arithmetic constraints */
      void p_int_plus(SolverInstanceBase& s, const Call* call);
      void p_int_minus(SolverInstanceBase& s, const Call* call);
      void p_int_times(SolverInstanceBase& s, const Call* call);
      void p_int_div(SolverInstanceBase& s, const Call* call);
      void p_int_mod(SolverInstanceBase& s, const Call* call);
      void p_int_min(SolverInstanceBase& s, const Call* call);
      void p_int_max(SolverInstanceBase& s, const Call* call);
      void p_int_negate(SolverInstanceBase& s, const Call* call) ;
      
      ///* Boolean constraints */
      void p_bool_CMP(GecodeSolverInstance& s, Gecode::IntRelType irt, const Call* call);
      void p_bool_CMP_reif(GecodeSolverInstance& s, Gecode::IntRelType irt, Gecode::ReifyMode rm, const Call* call);
      void p_bool_eq(SolverInstanceBase& s, const Call* call);
      void p_bool_eq_reif(SolverInstanceBase& s, const Call* call) ;
      void p_bool_eq_imp(SolverInstanceBase& s, const Call* call);
      void p_bool_ne(SolverInstanceBase& s, const Call* call);
      void p_bool_ne_reif(SolverInstanceBase& s, const Call* call);
      void p_bool_ne_imp(SolverInstanceBase& s, const Call* call);
      void p_bool_ge(SolverInstanceBase& s, const Call* call);
      void p_bool_ge_reif(SolverInstanceBase& s, const Call* call);
      void p_bool_ge_imp(SolverInstanceBase& s, const Call* call);
      void p_bool_le(SolverInstanceBase& s, const Call* call);
      void p_bool_le_reif(SolverInstanceBase& s, const Call* call);
      void p_bool_le_imp(SolverInstanceBase& s, const Call* call);
      void p_bool_gt(SolverInstanceBase& s, const Call* call);
      void p_bool_gt_reif(SolverInstanceBase& s, const Call* call);
      void p_bool_gt_imp(SolverInstanceBase& s, const Call* call);
      void p_bool_lt(SolverInstanceBase& s, const Call* call);
      void p_bool_lt_reif(SolverInstanceBase& s, const Call* call);
      void p_bool_lt_imp(SolverInstanceBase& s, const Call* call);
      void p_bool_or(SolverInstanceBase& s, const Call* call) ;
      void p_bool_or_imp(SolverInstanceBase& s, const Call* call);
      void p_bool_and(SolverInstanceBase& s, const Call* call);
      void p_bool_and_imp(SolverInstanceBase& s, const Call* call);
      void p_array_bool_and(SolverInstanceBase& s, const Call* call);
      void p_array_bool_and_imp(SolverInstanceBase& s, const Call* call);
      void p_array_bool_or(SolverInstanceBase& s, const Call* call);
      void p_array_bool_or_imp(SolverInstanceBase& s, const Call* call);
      void p_array_bool_xor(SolverInstanceBase& s, const Call* call);
      void p_array_bool_xor_imp(SolverInstanceBase& s, const Call* call);
      void p_array_bool_clause(SolverInstanceBase& s, const Call* call);
      void p_array_bool_clause_reif(SolverInstanceBase& s, const Call* call);
      void p_array_bool_clause_imp(SolverInstanceBase& s, const Call* call);
      void p_bool_xor(SolverInstanceBase& s, const Call* call);
      void p_bool_xor_imp(SolverInstanceBase& s, const Call* call);
      void p_bool_l_imp(SolverInstanceBase& s, const Call* call);
      void p_bool_r_imp(SolverInstanceBase& s, const Call* call);
      void p_bool_not(SolverInstanceBase& s, const Call* call);
      
      ///* element constraints */
      void p_array_int_element(SolverInstanceBase& s, const Call* call);
      void p_array_bool_element(SolverInstanceBase& s, const Call* call);
      
      ///* coercion constraints */
      void p_bool2int(SolverInstanceBase& s, const Call* call);
      void p_int_in(SolverInstanceBase& s, const Call* call);
      void p_int_in_reif(SolverInstanceBase& s, const Call* call);
      void p_int_in_imp(SolverInstanceBase& s, const Call* call);
      
      ///* constraints from the standard library */
      void p_abs(SolverInstanceBase& s, const Call* call);
      void p_array_int_lt(SolverInstanceBase& s, const Call* call);
      void p_array_int_lq(SolverInstanceBase& s, const Call* call);
      void p_array_bool_lt(SolverInstanceBase& s, const Call* call);
      void p_array_bool_lq(SolverInstanceBase& s, const Call* call);
      void p_count(SolverInstanceBase& s, const Call* call);
      void p_count_reif(SolverInstanceBase& s, const Call* call);
      void p_count_imp(SolverInstanceBase& s, const Call* call);
      void count_rel(Gecode::IntRelType irt, SolverInstanceBase& s, const Call* call);
      void p_at_most(SolverInstanceBase& s, const Call* call);
      void p_at_least(SolverInstanceBase& s, const Call* call);
      void p_bin_packing_load(SolverInstanceBase& s, const Call* call);
      void p_global_cardinality(SolverInstanceBase& s, const Call* call);
      void p_global_cardinality_closed(SolverInstanceBase& s, const Call* call);
      void p_global_cardinality_low_up(SolverInstanceBase& s, const Call* call);
      void p_global_cardinality_low_up_closed(SolverInstanceBase& s, const Call* call);
      void p_minimum(SolverInstanceBase& s, const Call* call);
      void p_maximum(SolverInstanceBase& s, const Call* call);
      void p_sort(SolverInstanceBase& s, const Call* call);
      void p_inverse_offsets(SolverInstanceBase& s, const Call* call);
      void p_increasing_int(SolverInstanceBase& s, const Call* call);
      void p_increasing_bool(SolverInstanceBase& s, const Call* call);
      void p_decreasing_int(SolverInstanceBase& s, const Call* call);
      void p_decreasing_bool(SolverInstanceBase& s, const Call* call);
      void p_table_int(SolverInstanceBase& s, const Call* call);
      void p_table_bool(SolverInstanceBase& s, const Call* call);
      void p_cumulatives(SolverInstanceBase& s, const Call* call);
      void p_among_seq_int(SolverInstanceBase& s, const Call* call);
      void p_among_seq_bool(SolverInstanceBase& s, const Call* call);
      void p_schedule_unary(SolverInstanceBase& s, const Call* call);
      void p_schedule_unary_optional(SolverInstanceBase& s, const Call* call);
      void p_circuit(SolverInstanceBase& s, const Call* call);
      void p_circuit_cost_array(SolverInstanceBase& s, const Call* call);
      void p_circuit_cost(SolverInstanceBase& s, const Call* call);
      void p_nooverlap(SolverInstanceBase& s, const Call* call);
      void p_precede(SolverInstanceBase& s, const Call* call);
      void p_nvalue(SolverInstanceBase& s, const Call* call);
      void p_among(SolverInstanceBase& s, const Call* call);
      void p_member_int(SolverInstanceBase& s, const Call* call);
      void p_member_int_reif(SolverInstanceBase& s, const Call* call);
      void p_member_bool(SolverInstanceBase& s, const Call* call);
      void p_member_bool_reif(SolverInstanceBase& s, const Call* call);
      
      #ifdef GECODE_HAS_FLOAT_VARS
      void p_int2float(SolverInstanceBase& s, const Call* ce);
      void p_float_lin_cmp(GecodeSolverInstance& s, Gecode::FloatRelType frt, const Call* ce);
      void p_float_lin_cmp_reif(GecodeSolverInstance& s, Gecode::FloatRelType frt, const Call* ce);
      void p_float_lin_eq(SolverInstanceBase& s, const Call* ce);
      void p_float_lin_eq_reif(SolverInstanceBase& s, const Call* ce);
      void p_float_lin_le(SolverInstanceBase& s, const Call* ce);
      void p_float_lin_le_reif(SolverInstanceBase& s, const Call* ce);
      void p_float_times(SolverInstanceBase& s, const Call* ce);
      void p_float_div(SolverInstanceBase& s, const Call* ce);
      void p_float_plus(SolverInstanceBase& s, const Call* ce) ;
      void p_float_sqrt(SolverInstanceBase& s, const Call* ce);
      void p_float_abs(SolverInstanceBase& s, const Call* ce);
      void p_float_eq(SolverInstanceBase& s, const Call* ce);
      void p_float_eq_reif(SolverInstanceBase& s, const Call* ce);
      void p_float_le(SolverInstanceBase& s, const Call* ce);
      void p_float_le_reif(SolverInstanceBase& s, const Call* ce);
      void p_float_max(SolverInstanceBase& s, const Call* ce);
      void p_float_min(SolverInstanceBase& s, const Call* ce);
      void p_float_lt(SolverInstanceBase& s, const Call* ce);
      void p_float_lt_reif(SolverInstanceBase& s, const Call* ce);
      void p_float_ne(SolverInstanceBase& s, const Call* ce);
      #ifdef GECODE_HAS_MPFR
#define P_FLOAT_OP(Op) \
      void p_float_ ## Op (SolverInstanceBase& s, const Call* ce) {\
	  GecodeSolverInstance& gi = (GecodeSolverInstance&)s; \
	  FloatVar x = gi.arg2FloatVar(ce->args()[0]);\
	  FloatVar y = gi.arg2FloatVar(ce->args()[1]);\
	  Op(gi ,x,y);\
      }
      P_FLOAT_OP(acos)
	  P_FLOAT_OP(asin)
	  P_FLOAT_OP(atan)
	  P_FLOAT_OP(cos)
	  P_FLOAT_OP(exp)
	  P_FLOAT_OP(sin)
	  P_FLOAT_OP(tan)         
#undef P_FLOAT_OP
      void p_float_ln(SolverInstanceBase& s, const Call* ce);
      void p_float_log10(SolverInstanceBase& s, const Call* ce);
      void p_float_log2(SolverInstanceBase& s, const Call* ce);	
      #endif	
      #endif	
	
    }
}

#endif