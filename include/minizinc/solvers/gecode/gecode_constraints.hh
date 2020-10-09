/*
 *  Main authors:
 *     Kevin Leo <kevin.leo@monash.edu>
 *     Andrea Rendl <andrea.rendl@nicta.com.au>
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <minizinc/solver_instance_base.hh>
#include <minizinc/solvers/gecode_solverinstance.hh>

namespace MiniZinc {

namespace GecodeConstraints {

#define PosterImpl(X) void X(SolverInstanceBase& s, const Call* ce)

PosterImpl(p_distinct);
PosterImpl(p_distinct_offset);
PosterImpl(p_all_equal);
void p_int_cmp(GecodeSolverInstance& s, Gecode::IntRelType irt, const Call* ce);
PosterImpl(p_int_eq);
PosterImpl(p_int_ne);
PosterImpl(p_int_ge);
PosterImpl(p_int_gt);
PosterImpl(p_int_le);
PosterImpl(p_int_lt);
void p_int_cmp_reif(GecodeSolverInstance& s, Gecode::IntRelType irt, Gecode::ReifyMode rm,
                    const Call* call);
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
void p_int_lin_cmp(GecodeSolverInstance& s, Gecode::IntRelType irt, const Call* call);
void p_int_lin_cmp_reif(GecodeSolverInstance& s, Gecode::IntRelType irt, Gecode::ReifyMode rm,
                        const Call* call);
PosterImpl(p_int_lin_eq);
PosterImpl(p_int_lin_eq_reif);
PosterImpl(p_int_lin_eq_imp);
PosterImpl(p_int_lin_ne);
PosterImpl(p_int_lin_ne_reif);
PosterImpl(p_int_lin_ne_imp);
PosterImpl(p_int_lin_le);
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
void p_bool_lin_cmp(GecodeSolverInstance& s, Gecode::IntRelType irt, const Call* call);
void p_bool_lin_cmp_reif(GecodeSolverInstance& s, Gecode::IntRelType irt, Gecode::ReifyMode rm,
                         const Call* call);
PosterImpl(p_bool_lin_eq);
PosterImpl(p_bool_lin_eq_reif);
PosterImpl(p_bool_lin_eq_imp);
PosterImpl(p_bool_lin_ne);
PosterImpl(p_bool_lin_ne_reif);
PosterImpl(p_bool_lin_ne_imp);
PosterImpl(p_bool_lin_le);
PosterImpl(p_bool_lin_le_reif);
PosterImpl(p_bool_lin_le_imp);
PosterImpl(p_bool_lin_lt);
PosterImpl(p_bool_lin_lt_reif);
PosterImpl(p_bool_lin_lt_imp);
PosterImpl(p_bool_lin_ge);
PosterImpl(p_bool_lin_ge_reif);
PosterImpl(p_bool_lin_ge_imp);
PosterImpl(p_bool_lin_gt);
PosterImpl(p_bool_lin_gt_reif);
PosterImpl(p_bool_lin_gt_imp);

///* arithmetic constraints */
PosterImpl(p_int_plus);
PosterImpl(p_int_minus);
PosterImpl(p_int_times);
PosterImpl(p_int_div);
PosterImpl(p_int_mod);
PosterImpl(p_int_min);
PosterImpl(p_int_max);
PosterImpl(p_int_negate);

///* Boolean constraints */
void p_bool_cmp(GecodeSolverInstance& s, Gecode::IntRelType irt, const Call* call);
void p_bool_cmp_reif(GecodeSolverInstance& s, Gecode::IntRelType irt, Gecode::ReifyMode rm,
                     const Call* call);
PosterImpl(p_bool_eq);
PosterImpl(p_bool_eq_reif);
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
PosterImpl(p_bool_or);
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
PosterImpl(p_float_plus);
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
PosterImpl(p_float_acos);
PosterImpl(p_float_asin);
PosterImpl(p_float_atan);
PosterImpl(p_float_cos);
PosterImpl(p_float_exp);
PosterImpl(p_float_sin);
PosterImpl(p_float_tan);
PosterImpl(p_float_ln);
PosterImpl(p_float_log10);
PosterImpl(p_float_log2);
#endif
#endif

#ifdef GECODE_HAS_SET_VARS
PosterImpl(p_set_eq);
PosterImpl(p_set_le);
PosterImpl(p_set_lt);
PosterImpl(p_set_eq);
PosterImpl(p_set_ne);
PosterImpl(p_set_union);
PosterImpl(p_array_set_element);
PosterImpl(p_array_set_element);
PosterImpl(p_set_intersect);
PosterImpl(p_set_diff);
PosterImpl(p_set_symdiff);
PosterImpl(p_set_subset);
PosterImpl(p_set_superset);
PosterImpl(p_set_card);
PosterImpl(p_set_in);
PosterImpl(p_set_eq_reif);
PosterImpl(p_set_le_reif);
PosterImpl(p_set_lt_reif);
PosterImpl(p_set_eq_reif);
PosterImpl(p_set_ne_reif);
PosterImpl(p_set_subset_reif);
PosterImpl(p_set_superset_reif);
PosterImpl(p_set_in_reif);
PosterImpl(p_set_in_imp);
PosterImpl(p_set_disjoint);
PosterImpl(p_link_set_to_booleans);
PosterImpl(p_array_set_union);
PosterImpl(p_array_set_partition);
PosterImpl(p_set_convex);
PosterImpl(p_array_set_seq);
PosterImpl(p_array_set_seq_union);
PosterImpl(p_array_set_element_union);
PosterImpl(p_array_set_element_intersect);
PosterImpl(p_array_set_element_intersect_in);
PosterImpl(p_array_set_element_partition);
PosterImpl(p_int_set_channel);
PosterImpl(p_range);
PosterImpl(p_weights);
PosterImpl(p_inverse_set);
PosterImpl(p_precede_set);
#endif

}  // namespace GecodeConstraints
}  // namespace MiniZinc
