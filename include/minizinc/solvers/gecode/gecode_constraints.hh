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
#include <minizinc/solvers/gecode_solverinstance.hh>

namespace MiniZinc {

namespace GecodeConstraints {

#ifndef HAS_GECODE_VERSION_5_1
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
#endif

#define PosterImpl(X) void X(SolverInstanceBase& s, const Call* ce)

      PosterImpl(p_distinct);
      PosterImpl(p_distinctOffset);
      PosterImpl(p_all_equal);
      void p_int_CMP(GecodeSolverInstance& s, Gecode::IntRelType irt, const Call* ce);
      PosterImpl(p_int_eq);
      PosterImpl(p_int_ne);
      PosterImpl(p_int_ge);
      PosterImpl(p_int_gt);
      PosterImpl(p_int_le);
      PosterImpl(p_int_lt);
      void p_int_CMP_reif(GecodeSolverInstance& s, Gecode::IntRelType irt, Gecode::ReifyMode rm, const Call* call);
      ///* Comparisons */
      PosterImpl(p_int_eq_reif);
      PosterImpl(p_int_ne_reif);
      PosterImpl(p_int_ge_reif);
      PosterImpl(p_int_gt_reif);
      PosterImpl(p_int_le_reif);
      PosterImpl(p_int_lt_reif);
      PosterImpl(p_int_eq_imp);
      PosterImpl(p_int_ne_imp);
      PosterImpl(p_int_ge_imp);
      PosterImpl(p_int_gt_imp);
      PosterImpl(p_int_le_imp);
      PosterImpl(p_int_lt_imp);
      void p_int_lin_CMP(GecodeSolverInstance& s, Gecode::IntRelType irt, const Call* call) ;
      void p_int_lin_CMP_reif(GecodeSolverInstance& s, Gecode::IntRelType irt, Gecode::ReifyMode rm, const Call* call);
      PosterImpl(p_int_lin_eq);
      PosterImpl(p_int_lin_eq_reif);
      PosterImpl(p_int_lin_eq_imp);
      PosterImpl(p_int_lin_ne);
      PosterImpl(p_int_lin_ne_reif);
      PosterImpl(p_int_lin_ne_imp);
      PosterImpl(p_int_lin_le) ;
      PosterImpl(p_int_lin_le_reif);
      PosterImpl(p_int_lin_le_imp);
      PosterImpl(p_int_lin_lt);
      PosterImpl(p_int_lin_lt_reif);
      PosterImpl(p_int_lin_lt_imp);
      PosterImpl(p_int_lin_ge);
      PosterImpl(p_int_lin_ge_reif);
      PosterImpl(p_int_lin_ge_imp);
      PosterImpl(p_int_lin_gt);
      PosterImpl(p_int_lin_gt_reif);
      PosterImpl(p_int_lin_gt_imp);
      void p_bool_lin_CMP(GecodeSolverInstance& s, Gecode::IntRelType irt, const Call* call);
      void p_bool_lin_CMP_reif(GecodeSolverInstance& s, Gecode::IntRelType irt, Gecode::ReifyMode rm, const Call* call);
      PosterImpl(p_bool_lin_eq);
      PosterImpl(p_bool_lin_eq_reif);
      PosterImpl(p_bool_lin_eq_imp) ;
      PosterImpl(p_bool_lin_ne);
      PosterImpl(p_bool_lin_ne_reif) ;
      PosterImpl(p_bool_lin_ne_imp) ;
      PosterImpl(p_bool_lin_le);
      PosterImpl(p_bool_lin_le_reif) ;
      PosterImpl(p_bool_lin_le_imp) ;
      PosterImpl(p_bool_lin_lt) ;
      PosterImpl(p_bool_lin_lt_reif);
      PosterImpl(p_bool_lin_lt_imp) ;
      PosterImpl(p_bool_lin_ge);
      PosterImpl(p_bool_lin_ge_reif) ;
      PosterImpl(p_bool_lin_ge_imp) ;
      PosterImpl(p_bool_lin_gt) ;
      PosterImpl(p_bool_lin_gt_reif) ;
      PosterImpl(p_bool_lin_gt_imp) ;
      
      ///* arithmetic constraints */
      PosterImpl(p_int_plus);
      PosterImpl(p_int_minus);
      PosterImpl(p_int_times);
      PosterImpl(p_int_div);
      PosterImpl(p_int_mod);
      PosterImpl(p_int_min);
      PosterImpl(p_int_max);
      PosterImpl(p_int_negate) ;
      
      ///* Boolean constraints */
      void p_bool_CMP(GecodeSolverInstance& s, Gecode::IntRelType irt, const Call* call);
      void p_bool_CMP_reif(GecodeSolverInstance& s, Gecode::IntRelType irt, Gecode::ReifyMode rm, const Call* call);
      PosterImpl(p_bool_eq);
      PosterImpl(p_bool_eq_reif) ;
      PosterImpl(p_bool_eq_imp);
      PosterImpl(p_bool_ne);
      PosterImpl(p_bool_ne_reif);
      PosterImpl(p_bool_ne_imp);
      PosterImpl(p_bool_ge);
      PosterImpl(p_bool_ge_reif);
      PosterImpl(p_bool_ge_imp);
      PosterImpl(p_bool_le);
      PosterImpl(p_bool_le_reif);
      PosterImpl(p_bool_le_imp);
      PosterImpl(p_bool_gt);
      PosterImpl(p_bool_gt_reif);
      PosterImpl(p_bool_gt_imp);
      PosterImpl(p_bool_lt);
      PosterImpl(p_bool_lt_reif);
      PosterImpl(p_bool_lt_imp);
      PosterImpl(p_bool_or) ;
      PosterImpl(p_bool_or_imp);
      PosterImpl(p_bool_and);
      PosterImpl(p_bool_and_imp);
      PosterImpl(p_array_bool_and);
      PosterImpl(p_array_bool_and_imp);
      PosterImpl(p_array_bool_or);
      PosterImpl(p_array_bool_or_imp);
      PosterImpl(p_array_bool_xor);
      PosterImpl(p_array_bool_xor_imp);
      PosterImpl(p_array_bool_clause);
      PosterImpl(p_array_bool_clause_reif);
      PosterImpl(p_array_bool_clause_imp);
      PosterImpl(p_bool_xor);
      PosterImpl(p_bool_xor_imp);
      PosterImpl(p_bool_l_imp);
      PosterImpl(p_bool_r_imp);
      PosterImpl(p_bool_not);
      
      ///* element constraints */
      PosterImpl(p_array_int_element);
      PosterImpl(p_array_bool_element);
      
      ///* coercion constraints */
      PosterImpl(p_bool2int);
      PosterImpl(p_int_in);
      PosterImpl(p_int_in_reif);
      PosterImpl(p_int_in_imp);
      
      ///* constraints from the standard library */
      PosterImpl(p_abs);
      PosterImpl(p_array_int_lt);
      PosterImpl(p_array_int_lq);
      PosterImpl(p_array_bool_lt);
      PosterImpl(p_array_bool_lq);
      PosterImpl(p_count);
      PosterImpl(p_count_reif);
      PosterImpl(p_count_imp);
      void count_rel(Gecode::IntRelType irt, SolverInstanceBase& s, const Call* call);
      PosterImpl(p_at_most);
      PosterImpl(p_at_least);
      PosterImpl(p_bin_packing_load);
      PosterImpl(p_global_cardinality);
      PosterImpl(p_global_cardinality_closed);
      PosterImpl(p_global_cardinality_low_up);
      PosterImpl(p_global_cardinality_low_up_closed);
      PosterImpl(p_minimum);
      PosterImpl(p_maximum);
      PosterImpl(p_minimum_arg);
      PosterImpl(p_maximum_arg);
      PosterImpl(p_regular);
      PosterImpl(p_sort);
      PosterImpl(p_inverse_offsets);
      PosterImpl(p_increasing_int);
      PosterImpl(p_increasing_bool);
      PosterImpl(p_decreasing_int);
      PosterImpl(p_decreasing_bool);
      PosterImpl(p_table_int);
      PosterImpl(p_table_bool);
      PosterImpl(p_cumulatives);
      PosterImpl(p_among_seq_int);
      PosterImpl(p_among_seq_bool);
      PosterImpl(p_schedule_unary);
      PosterImpl(p_schedule_unary_optional);
      PosterImpl(p_cumulative_opt);
      PosterImpl(p_circuit);
      PosterImpl(p_circuit_cost_array);
      PosterImpl(p_circuit_cost);
      PosterImpl(p_nooverlap);
      PosterImpl(p_precede);
      PosterImpl(p_nvalue);
      PosterImpl(p_among);
      PosterImpl(p_member_int);
      PosterImpl(p_member_int_reif);
      PosterImpl(p_member_bool);
      PosterImpl(p_member_bool_reif);
      
      #ifdef GECODE_HAS_FLOAT_VARS
      PosterImpl(p_int2float);
      void p_float_lin_cmp(GecodeSolverInstance& s, Gecode::FloatRelType frt, const Call* ce);
      void p_float_lin_cmp_reif(GecodeSolverInstance& s, Gecode::FloatRelType frt, const Call* ce);
      PosterImpl(p_float_lin_eq);
      PosterImpl(p_float_lin_eq_reif);
      PosterImpl(p_float_lin_le);
      PosterImpl(p_float_lin_le_reif);
      PosterImpl(p_float_times);
      PosterImpl(p_float_div);
      PosterImpl(p_float_plus) ;
      PosterImpl(p_float_sqrt);
      PosterImpl(p_float_abs);
      PosterImpl(p_float_eq);
      PosterImpl(p_float_eq_reif);
      PosterImpl(p_float_le);
      PosterImpl(p_float_le_reif);
      PosterImpl(p_float_max);
      PosterImpl(p_float_min);
      PosterImpl(p_float_lt);
      PosterImpl(p_float_lt_reif);
      PosterImpl(p_float_ne);
      #ifdef GECODE_HAS_MPFR
#define P_FLOAT_OP(Op) \
      PosterImpl(p_float_ ## Op ) {\
	  GecodeSolverInstance& gi = (GecodeSolverInstance&)s; \
	  Gecode::FloatVar x = gi.arg2floatvar(ce->args()[0]);\
	  Gecode::FloatVar y = gi.arg2floatvar(ce->args()[1]);\
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
      PosterImpl(p_float_ln);
      PosterImpl(p_float_log10);
      PosterImpl(p_float_log2);	
      #endif	
      #endif	
	
    }
}

#endif
