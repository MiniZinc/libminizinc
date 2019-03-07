/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Jip J. Dekker <jip.dekker@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MINIZINC_GEAS_CONSTRAINTS_HH__
#define __MINIZINC_GEAS_CONSTRAINTS_HH__

#include <minizinc/ast.hh>
#include <minizinc/solver_instance_base.hh>

namespace MiniZinc {
  namespace GeasConstraints {

#define PosterImpl(X) void X(SolverInstanceBase& s, const Call* ce)

    /* Integer Comparisons Constraints */
    PosterImpl(p_int_eq);
    PosterImpl(p_int_ne);
    PosterImpl(p_int_le);
    PosterImpl(p_int_lt);
    PosterImpl(p_int_eq_imp);
    PosterImpl(p_int_ne_imp);
    PosterImpl(p_int_le_imp);
    PosterImpl(p_int_lt_imp);
    PosterImpl(p_int_eq_reif);
    PosterImpl(p_int_ne_reif);
    PosterImpl(p_int_le_reif);
    PosterImpl(p_int_lt_reif);

    /* Integer Arithmetic Constraints */
    PosterImpl(p_int_abs);
    PosterImpl(p_int_times);
    PosterImpl(p_int_div);
    PosterImpl(p_int_max);
    PosterImpl(p_int_min);

    /* Integer Linear Constraints */
    PosterImpl(p_int_lin_eq);
    PosterImpl(p_int_lin_ne);
    PosterImpl(p_int_lin_le);
    PosterImpl(p_int_lin_eq_imp);
    PosterImpl(p_int_lin_ne_imp);
    PosterImpl(p_int_lin_le_imp);
    PosterImpl(p_int_lin_eq_reif);
    PosterImpl(p_int_lin_ne_reif);
    PosterImpl(p_int_lin_le_reif);

    /* Boolean Comparison Constraints */
    PosterImpl(p_bool_eq);
    PosterImpl(p_bool_ne);
    PosterImpl(p_bool_le);
    PosterImpl(p_bool_lt);
    PosterImpl(p_bool_eq_imp);
    PosterImpl(p_bool_ne_imp);
    PosterImpl(p_bool_le_imp);
    PosterImpl(p_bool_lt_imp);
    PosterImpl(p_bool_eq_reif);
    PosterImpl(p_bool_ne_reif);
    PosterImpl(p_bool_le_reif);
    PosterImpl(p_bool_lt_reif);

    /* Boolean Arithmetic Constraints */
    PosterImpl(p_bool_or) ;
    PosterImpl(p_bool_and);
    PosterImpl(p_bool_xor);
    PosterImpl(p_bool_not);
    PosterImpl(p_bool_or_imp);
    PosterImpl(p_bool_and_imp);
    PosterImpl(p_bool_xor_imp);

    PosterImpl(p_bool_clause);
    PosterImpl(p_array_bool_or);
    PosterImpl(p_array_bool_and);
//    PosterImpl(p_array_bool_xor);
    PosterImpl(p_bool_clause_imp);
    PosterImpl(p_array_bool_and_imp);
    PosterImpl(p_array_bool_or_imp);
//    PosterImpl(p_array_bool_xor_imp);
    PosterImpl(p_bool_clause_reif);

      /* Boolean Linear Constraints */
    PosterImpl(p_bool_lin_eq);
    PosterImpl(p_bool_lin_ne);
    PosterImpl(p_bool_lin_le);
    PosterImpl(p_bool_lin_eq_imp);
    PosterImpl(p_bool_lin_ne_imp);
    PosterImpl(p_bool_lin_le_imp);
    PosterImpl(p_bool_lin_lt_imp);
    PosterImpl(p_bool_lin_eq_reif);
    PosterImpl(p_bool_lin_ne_reif);
    PosterImpl(p_bool_lin_le_reif);

    /* Floating point constraints */
//    PosterImpl(p_float_lin_eq);
//    PosterImpl(p_float_lin_eq_reif);
//    PosterImpl(p_float_lin_le);
//    PosterImpl(p_float_lin_le_reif);
//    PosterImpl(p_float_times);
//    PosterImpl(p_float_div);
//    PosterImpl(p_float_plus) ;
//    PosterImpl(p_float_sqrt);
//    PosterImpl(p_float_abs);
//    PosterImpl(p_float_eq);
//    PosterImpl(p_float_eq_reif);
//    PosterImpl(p_float_le);
//    PosterImpl(p_float_le_reif);
//    PosterImpl(p_float_max);
//    PosterImpl(p_float_min);
//    PosterImpl(p_float_lt);
//    PosterImpl(p_float_lt_reif);
//    PosterImpl(p_float_ne);
//    PosterImpl(p_float_acos);
//    PosterImpl(p_float_asin);
//    PosterImpl(p_float_atan);
//    PosterImpl(p_float_cos);
//    PosterImpl(p_float_exp);
//    PosterImpl(p_float_sin);
//    PosterImpl(p_float_tan);
//    PosterImpl(p_float_ln);
//    PosterImpl(p_float_log10);
//    PosterImpl(p_float_log2);

    /* Element Constraints */
//    PosterImpl(p_array_int_element);
//    PosterImpl(p_array_bool_element);

    /* Coercion Constraints */
//    PosterImpl(p_int2float);
//    PosterImpl(p_bool2int);

//    PosterImpl(p_distinct);
//    PosterImpl(p_distinctOffset);
//    PosterImpl(p_all_equal);

    /* Global Constraints */
//    PosterImpl(p_array_int_lt);
//    PosterImpl(p_array_int_lq);
//    PosterImpl(p_array_bool_lt);
//    PosterImpl(p_array_bool_lq);
//    PosterImpl(p_count);
//    PosterImpl(p_count_reif);
//    PosterImpl(p_count_imp);
//    PosterImpl(p_at_most);
//    PosterImpl(p_at_least);
//    PosterImpl(p_bin_packing_load);
//    PosterImpl(p_global_cardinality);
//    PosterImpl(p_global_cardinality_closed);
//    PosterImpl(p_global_cardinality_low_up);
//    PosterImpl(p_global_cardinality_low_up_closed);
//    PosterImpl(p_minimum);
//    PosterImpl(p_maximum);
//    PosterImpl(p_minimum_arg);
//    PosterImpl(p_maximum_arg);
//    PosterImpl(p_regular);
//    PosterImpl(p_sort);
//    PosterImpl(p_inverse_offsets);
//    PosterImpl(p_increasing_int);
//    PosterImpl(p_increasing_bool);
//    PosterImpl(p_decreasing_int);
//    PosterImpl(p_decreasing_bool);
//    PosterImpl(p_table_int);
//    PosterImpl(p_table_bool);
//    PosterImpl(p_cumulatives);
//    PosterImpl(p_among_seq_int);
//    PosterImpl(p_among_seq_bool);
//    PosterImpl(p_schedule_unary);
//    PosterImpl(p_schedule_unary_optional);
//    PosterImpl(p_cumulative_opt);
//    PosterImpl(p_circuit);
//    PosterImpl(p_circuit_cost_array);
//    PosterImpl(p_circuit_cost);
//    PosterImpl(p_nooverlap);
//    PosterImpl(p_precede);
//    PosterImpl(p_nvalue);
//    PosterImpl(p_among);
//    PosterImpl(p_member_int);
//    PosterImpl(p_member_int_reif);
//    PosterImpl(p_member_bool);
//    PosterImpl(p_member_bool_reif);

  }
}

#endif // __MINIZINC_GEAS_CONSTRAINTS_HH__