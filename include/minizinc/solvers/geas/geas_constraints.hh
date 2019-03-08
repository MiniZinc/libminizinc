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

    /* Integer Comparison Constraints */
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
    PosterImpl(p_bool_clause_imp);
    PosterImpl(p_array_bool_and_imp);
    PosterImpl(p_array_bool_or_imp);
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

    /* Coercion Constraints */
    PosterImpl(p_bool2int);

    /* Element Constraints */
    PosterImpl(p_array_int_element);
    PosterImpl(p_array_bool_element);
    PosterImpl(p_array_int_maximum);
    PosterImpl(p_array_int_minimum);
    PosterImpl(p_array_var_int_element);
    PosterImpl(p_array_var_bool_element);

    /* Global Constraints */
    PosterImpl(p_all_different);
    PosterImpl(p_all_different_except_0);
    PosterImpl(p_at_most);
    PosterImpl(p_at_most1);
    PosterImpl(p_cumulative);
    PosterImpl(p_disjunctive);
    PosterImpl(p_global_cardinality);
    PosterImpl(p_table_int);

    /**** NOT YET SUPPORTED: ****/

    /* Boolean Arithmetic Constraints */
//    PosterImpl(p_array_bool_xor);
//    PosterImpl(p_array_bool_xor_imp);

    /* Floating Point Comparison Constraints */
//    PosterImpl(p_float_eq);
//    PosterImpl(p_float_ne);
//    PosterImpl(p_float_le);
//    PosterImpl(p_float_lt);
//    PosterImpl(p_float_eq_reif);
//    PosterImpl(p_float_le_reif)
//    PosterImpl(p_float_lt_reif);

    /* Floating Point Arithmetic Constraints */
//    PosterImpl(p_float_times);
//    PosterImpl(p_float_div);
//    PosterImpl(p_float_plus) ;
//    PosterImpl(p_float_sqrt);
//    PosterImpl(p_float_abs);;
//    PosterImpl(p_float_max);
//    PosterImpl(p_float_min);
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

    /* Floating Linear Constraints */
//    PosterImpl(p_float_lin_eq);
//    PosterImpl(p_float_lin_eq_reif);
//    PosterImpl(p_float_lin_le);
//    PosterImpl(p_float_lin_le_reif);

    /* Coercion Constraints */
//    PosterImpl(p_int2float);

  }
}

#endif // __MINIZINC_GEAS_CONSTRAINTS_HH__