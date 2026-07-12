%-----------------------------------------------------------------------------%
%
% FlatZinc builtins
%
%  This section contains declarations for the standard FlatZinc builtins.
%  They can be redefined by providing a custom redefinitions.mzn in the
%  solver globals library.
%

include "stdlib/stdlib_ann.mzn";

/***
  @groupdef flatzinc FlatZinc builtins

  These are the standard constraints that need to be supported by FlatZinc
  solvers (or redefined in the redefinitions.mzn file).
*/

/***
  @groupdef flatzinc.int Integer FlatZinc builtins
*/

/** @group flatzinc.int Constrains \a y to be the absolute value of \a x */
predicate int_abs(var int: x, var int: y);
/** @group flatzinc.int Constrains \a x to be equal to \a y */
predicate int_eq(var int: x, var int: y) :: promise_commutative;
/** @group flatzinc.int Constrains (\a x=\a y) \( \leftrightarrow \) \a b */
predicate int_eq_reif(var int: x, var int: y, var bool: b);
/** @group flatzinc.int Constrains \a x to be less than or equal to \a y */
predicate int_le(var int: x, var int: y);
/** @group flatzinc.int Constrains (\a x ≤ \a y) \( \leftrightarrow \) \a b */
predicate int_le_reif(var int: x, var int: y, var bool: b);
/** @group flatzinc.int Constrains \( \a y = \sum_i \a as[i]*\a xs[i] \) */
predicate int_lin_eq(array [int] of int: as, array [int] of var int: xs, int: y);
/** @group flatzinc.int Constrains \( \a b \leftrightarrow (\a y = \sum_i \a as[i]*\a xs[i]) \) */
predicate int_lin_eq_reif(array [int] of int: as, array [int] of var int: xs, int: y, var bool: b);
/** @group flatzinc.int Constrains \( \a y \neq \sum_i \a as[i]*\a xs[i] \) */
predicate int_lin_ne(array [int] of int: as, array [int] of var int: xs, int: y);
/** @group flatzinc.int Constrains \( \a b \leftrightarrow (\a y \neq \sum_i \a as[i]*\a xs[i]) \) */
predicate int_lin_ne_reif(array [int] of int: as, array [int] of var int: xs, int: y, var bool: b);

/** @group flatzinc.int Constrains \( \sum \) \a as[\p i]*\a xs[\p i] ≤ \a y */
predicate int_lin_le(array [int] of int: as, array [int] of var int: xs, int: y);
/** @group flatzinc.int Constrains \a b \( \leftrightarrow \) (\( \sum \) \a as[\p i]*\a xs[\p i] ≤ \a y) */
predicate int_lin_le_reif(array [int] of int: as, array [int] of var int: xs, int: y, var bool: b);
/** @group flatzinc.int Constrains \a x ≠ \a y */
predicate int_ne(var int: x, var int: y) :: promise_commutative;
/** @group flatzinc.int \a b \( \leftrightarrow \) (\a x ≠ \a y) */
predicate int_ne_reif(var int: x, var int: y, var bool: b);
/** @group flatzinc.int Constrains \a x + \a y = \a z */
predicate int_plus(var int: x, var int: y, var int: z);
/** @group flatzinc.int Constrains \a x / \a y = \a z, where \a z is rounded to the value closest to zero (i.e., truncated). */
predicate int_div(var int: x, var int: y, var int: z);
/** @group flatzinc.int Constrains \a x < \a y */
predicate int_lt(var int: x, var int: y);
/** @group flatzinc.int Constrains \a b \( \leftrightarrow \) (\a x < \a y) */
predicate int_lt_reif(var int: x, var int: y, var bool: b);
/** @group flatzinc.int Constrains max(\a x, \a y) = \a z */
predicate int_max(var int: x, var int: y, var int: z);
/** @group flatzinc.int Constrains min(\a x, \a y) = \a z */
predicate int_min(var int: x, var int: y, var int: z);
/** @group flatzinc.int Constrains \a x % \a y = \a z */
predicate int_mod(var int: x, var int: y, var int: z);
/** @group flatzinc.int Constrains \a x * \a y = \a z */
predicate int_times(var int: x, var int: y, var int: z);
/** @group flatzinc.int Constrains \a z = \(\a x ^ {\a y}\), \a z is constrained to ``1 div pow(x, abs(y))`` when \(\a y < 0\) */
predicate int_pow(var int: x, var int: y, var int: z);

/** @group flatzinc.int Constrains \a x \( \in \) \a s */
predicate set_in(var int: x, set of int: s);

/***
  @groupdef flatzinc.bool Bool FlatZinc builtins
*/

/** @group flatzinc.bool Constrains \( \a y \in \{0,1\} \) and \( \a x \leftrightarrow \a y=1 \) */
predicate bool2int(var bool: x, var int: y);
/** @group flatzinc.bool Constrains \( \a b \leftrightarrow \a x \land \a y \) */
predicate bool_and(var bool: x, var bool: y, var bool: b);
/** @group flatzinc.bool Constrains \( \bigvee_i \a xs[i] \lor \bigvee_j \lnot \a ys[j] \) */
predicate bool_clause(
  array [int] of var bool: xs :: promise_ctx_monotone,
  array [int] of var bool: ys :: promise_ctx_antitone,
);

/** @group flatzinc.bool Constrains \a x = \a y */
predicate bool_eq(var bool: x, var bool: y) :: promise_commutative;
/** @group flatzinc.bool Constrains \a b \( \leftrightarrow \) (\a x = \a y) */
predicate bool_eq_reif(var bool: x, var bool: y, var bool: b);
/** @group flatzinc.bool Constrains \a x ≤ \a y */
predicate bool_le(var bool: x :: promise_ctx_antitone, var bool: y :: promise_ctx_monotone);
/** @group flatzinc.bool Constrains \a b \( \leftrightarrow \) (\a x ≤ \a y) */
predicate bool_le_reif(var bool: x, var bool: y, var bool: b);
/** @group flatzinc.bool Constrains \( \a y = \sum_i \a as[i]*\a xs[i] \) */
predicate bool_lin_eq(array [int] of int: as, array [int] of var bool: xs, var int: y);
/** @group flatzinc.bool Constrains \( \sum_i \a as[i]*\a xs[i] \leq \a y \) */
predicate bool_lin_le(array [int] of int: as, array [int] of var bool: xs, int: y);
/** @group flatzinc.bool Constrains \a x < \a y */
predicate bool_lt(var bool: x :: promise_ctx_antitone, var bool: y :: promise_ctx_monotone);
/** @group flatzinc.bool Constrains \a b \( \leftrightarrow \) (\a x < \a y) */
predicate bool_lt_reif(var bool: x, var bool: y, var bool: b);
/** @group flatzinc.bool Constrains \a x ≠ \a y */
predicate bool_not(var bool: x, var bool: y);
/** @group flatzinc.bool Constrains \( \a b \leftrightarrow \a x \lor \a y \) */
predicate bool_or(var bool: x, var bool: y, var bool: b);
/** @group flatzinc.bool Constrains \( \a b \leftrightarrow \a x \oplus \a y \) */
predicate bool_xor(var bool: x, var bool: y, var bool: b);

/***
  @groupdef flatzinc.set Set FlatZinc builtins
*/

/** @group flatzinc.set Constrains \a x \( \in \) \a s */
predicate set_in(var int: x, var set of int: s);
/** @group flatzinc.set Constrains \a x = |\a s| */
predicate set_card(var set of int: s, var int: x);
/** @group flatzinc.set Constrains \( \a b \leftrightarrow (\a x \in \a s) \) */
predicate set_in_reif(var int: x, set of int: s, var bool: b);
/** @group flatzinc.set Constrains \( \a b \leftrightarrow (\a x \in \a s) \) */
predicate set_in_reif(var int: x, var set of int: s, var bool: b);
/** @group flatzinc.set Constrains \a s \( \subseteq \) \a t */
predicate set_subset(var set of int: s, var set of int: t);
/** @group flatzinc.set Constrains \a s \( \supseteq \) \a t */
predicate set_superset(var set of int: s, var set of int: t);
/** @group flatzinc.set Constrains \( \a b \leftrightarrow (\a s \subseteq \a t) \) */
predicate set_subset_reif(var set of int: s, var set of int: t, var bool: b);
/** @group flatzinc.set Constrains \( \a b \leftrightarrow (\a s \subseteq \a t) \) */
predicate set_superset_reif(var set of int: s, var set of int: t, var bool: b);
/** @group flatzinc.set Constrains \a s ≤ \a t (lexicographic order of the sorted lists of elements) */
predicate set_le(var set of int: s, var set of int: t);
/** @group flatzinc.set Constrains \( \a b \leftrightarrow (\a s \leq \a t) \) (lexicographic order of the sorted lists of elements) */
predicate set_le_reif(var set of int: s, var set of int: t, var bool: b);
/** @group flatzinc.set Constrains \a s < \a t (lexicographic order of the sorted lists of elements) */
predicate set_lt(var set of int: s, var set of int: t);
/** @group flatzinc.set Constrains \( \a b \leftrightarrow (\a s < \a t) \) (lexicographic order of the sorted lists of elements) */
predicate set_lt_reif(var set of int: s, var set of int: t, var bool: b);
/** @group flatzinc.set Constrains \a s = \a t */
predicate set_eq(var set of int: s, var set of int: t) :: promise_commutative;
/** @group flatzinc.set Constrains \a b \( \leftrightarrow \) (\a s = \a t) */
predicate set_eq_reif(var set of int: s, var set of int: t, var bool: b);
/** @group flatzinc.set Constrains \a s ≠ \a t */
predicate set_ne(var set of int: s, var set of int: t) :: promise_commutative;
/** @group flatzinc.set Constrains \a b \( \leftrightarrow \) (\a s ≠ \a t) */
predicate set_ne_reif(var set of int: s, var set of int: t, var bool: b);
/** @group flatzinc.set Constrains \a r = \a s \( \cap \) \a t */
predicate set_intersect(var set of int: s, var set of int: t, var set of int: r);
/** @group flatzinc.set Constrains \a r = \a s \( \cup \) \a t */
predicate set_union(var set of int: s, var set of int: t, var set of int: r);
/** @group flatzinc.set Constrains \a r = \a s \( \setminus \) \a t */
predicate set_diff(var set of int: s, var set of int: t, var set of int: r);
/** @group flatzinc.set Constrains \a r to be the symmetric difference of \a s and \a t */
predicate set_symdiff(var set of int: s, var set of int: t, var set of int: r);

/***
  @groupdef flatzinc.float Float FlatZinc builtins
*/

/** @group flatzinc.float Constrains \a y to be the absolute value of \a x */
predicate float_abs(var float: x, var float: y);
/** @group flatzinc.float Constrains \a y = acos(\a x) */
predicate float_acos(var float: x, var float: y);
/** @group flatzinc.float Constrains \a y = acosh(\a x) */
predicate float_acosh(var float: x, var float: y);
/** @group flatzinc.float Constrains \a y = asin(\a x) */
predicate float_asin(var float: x, var float: y);
/** @group flatzinc.float Constrains \a y = asinh(\a x) */
predicate float_asinh(var float: x, var float: y);
/** @group flatzinc.float Constrains \a y = atan(\a x) */
predicate float_atan(var float: x, var float: y);
/** @group flatzinc.float Constrains \a y = atanh(\a x) */
predicate float_atanh(var float: x, var float: y);
/** @group flatzinc.float Constrains \a y = cos(\a x) */
predicate float_cos(var float: x, var float: y);
/** @group flatzinc.float Constrains \a y = cosh(\a x) */
predicate float_cosh(var float: x, var float: y);
/** @group flatzinc.float Constrains \a y = exp(\a x) */
predicate float_exp(var float: x, var float: y);
/** @group flatzinc.float Constrains \a y = ln(\a x) */
predicate float_ln(var float: x, var float: y);
/** @group flatzinc.float Constrains \a y = log<sub>10</sub>(\a x) */
predicate float_log10(var float: x, var float: y);
/** @group flatzinc.float Constrains \a y = log<sub>2</sub>(\a x) */
predicate float_log2(var float: x, var float: y);
/** @group flatzinc.float Constrains \(\a y = \sqrt{\a x}\) */
predicate float_sqrt(var float: x, var float: y);
/** @group flatzinc.float Constrains \a z = \(\a x ^ {\a y}\) */
predicate float_pow(var float: x, var float: y, var float: z);
/** @group flatzinc.float Constrains \a y = sin(\a x) */
predicate float_sin(var float: x, var float: y);
/** @group flatzinc.float Constrains \a y = sinh(\a x) */
predicate float_sinh(var float: x, var float: y);
/** @group flatzinc.float Constrains \a y = tan(\a x) */
predicate float_tan(var float: x, var float: y);
/** @group flatzinc.float Constrains \a y = tanh(\a x) */
predicate float_tanh(var float: x, var float: y);

/** @group flatzinc.float Constrains \a x = \a y */
predicate float_eq(var float: x, var float: y) :: promise_commutative;
/** @group flatzinc.float Constrains \a b \( \leftrightarrow \) (\a x = \a y) */
predicate float_eq_reif(var float: x, var float: y, var bool: b);
/** @group flatzinc.float Constrains \a x ≤ \a y */
predicate float_le(var float: x, var float: y);
/** @group flatzinc.float Constrains \a b \( \leftrightarrow \) (\a x ≤ \a y) */
predicate float_le_reif(var float: x, var float: y, var bool: b);
/** @group flatzinc.float Constrains \a x < \a y */
predicate float_lt(var float: x, var float: y);
/** @group flatzinc.float Constrains \a b \( \leftrightarrow \) (\a x < \a y) */
predicate float_lt_reif(var float: x, var float: y, var bool: b);
/** @group flatzinc.float Constrains \a x ≠ \a y */
predicate float_ne(var float: x, var float: y) :: promise_commutative;
/** @group flatzinc.float Constrains \a b \( \leftrightarrow \) (\a x ≠ \a y) */
predicate float_ne_reif(var float: x, var float: y, var bool: b);

/** @group flatzinc.float Constrains \( \a y = \sum_i \a as[i]*\a xs[i] \) */
predicate float_lin_eq(array [int] of float: as, array [int] of var float: xs, float: y);
/** @group flatzinc.float Constrains \( \a b \leftrightarrow (\a y = \sum_i \a as[i]*\a xs[i]) \) */
predicate float_lin_eq_reif(
  array [int] of float: as,
  array [int] of var float: xs,
  float: y,
  var bool: b,
);
/** @group flatzinc.float Constrains \( \sum_i \a as[i]*\a xs[i] \leq \a y \) */
predicate float_lin_le(array [int] of float: as, array [int] of var float: xs, float: y);
/** @group flatzinc.float Constrains \( \a b \leftrightarrow (\sum_i \a as[i]*\a xs[i] \leq \a y) \) */
predicate float_lin_le_reif(
  array [int] of float: as,
  array [int] of var float: xs,
  float: y,
  var bool: b,
);
/** @group flatzinc.float Constrains \( \sum_i \a as[i]*\a xs[i] < \a y \) */
predicate float_lin_lt(array [int] of float: as, array [int] of var float: xs, float: y);
/** @group flatzinc.float Constrains \( \a b \leftrightarrow (\sum_i \a as[i]*\a xs[i] < \a y) \) */
predicate float_lin_lt_reif(
  array [int] of float: as,
  array [int] of var float: xs,
  float: y,
  var bool: b,
);
/** @group flatzinc.float Constrains \( \a y \neq \sum_i \a as[i]*\a xs[i] \) */
predicate float_lin_ne(array [int] of float: as, array [int] of var float: xs, float: y);
/** @group flatzinc.float Constrains \( \a b \leftrightarrow (\a y \neq \sum_i \a as[i]*\a xs[i]) \) */
predicate float_lin_ne_reif(
  array [int] of float: as,
  array [int] of var float: xs,
  float: y,
  var bool: b,
);

/** @group flatzinc.float Constrains max(\a x, \a y) = \a z */
predicate float_max(var float: x, var float: y, var float: z);
/** @group flatzinc.float Constrains min(\a x, \a y) = \a z */
predicate float_min(var float: x, var float: y, var float: z);
/** @group flatzinc.float Constrains \a x + \a y = \a z */
predicate float_plus(var float: x, var float: y, var float: z);
/** @group flatzinc.float Constrains \a x / \a y = \a z */
predicate float_div(var float: x, var float: y, var float: z);
/** @group flatzinc.float Constrains \a x * \a y = \a z */
predicate float_times(var float: x, var float: y, var float: z);

/** @group flatzinc.float Constrains \a y=\a x */
predicate int2float(var int: x, var float: y);

% Array constraints

/** @group flatzinc.bool Constrains \( \a b \leftrightarrow \bigwedge_i \a xs[i]\) */
predicate array_bool_and(array [int] of var bool: xs, var bool: b);
/** @group flatzinc.bool Constrains \( \oplus_i\ \a xs[i]\) */
predicate array_bool_xor(array [int] of var bool: xs);
/** @group flatzinc.bool Constrains \a xs[\a idx] = \a y */
predicate array_bool_element(var int: idx, array [int] of bool: xs, var bool: y);
/** @group flatzinc.int Constrains \a xs[\a idx] = \a y */
predicate array_int_element(var int: idx, array [int] of int: xs, var int: y);
/** @group flatzinc.float Constrains \a xs[\a idx] = \a y */
predicate array_float_element(var int: idx, array [int] of float: xs, var float: y);
/** @group flatzinc.set Constrains \a xs[\a idx] = \a y */
predicate array_set_element(var int: idx, array [int] of set of int: xs, var set of int: y);
/** @group flatzinc.bool Constrains \a xs[\a idx] = \a y */
predicate array_var_bool_element(var int: idx, array [int] of var bool: xs, var bool: y);
/** @group flatzinc.int Constrains \a xs[\a idx] = \a y */
predicate array_var_int_element(var int: idx, array [int] of var int: xs, var int: y);
/** @group flatzinc.float Constrains \a xs[\a idx] = \a y */
predicate array_var_float_element(var int: idx, array [int] of var float: xs, var float: y);
/** @group flatzinc.set Constrains \a xs[\a idx] = \a y */
predicate array_var_set_element(var int: idx, array [int] of var set of int: xs, var set of int: y);

/***
  @groupdef flatzinc.two FlatZinc builtins added in MiniZinc 2.0.0.

  These functions and predicates define built-in operations of the MiniZinc language
  that have been added in MiniZinc 2.0.0. Solvers that support these natively need
  to include a file called redefinitions-2.0.mzn in their solver library that
  redefines these predicates as builtins.

*/

/** @group flatzinc.two Reified clause constraint.
  Constrains \( \a b \leftrightarrow \bigvee_i \a xs[i] \lor \bigvee_j \lnot \a ys[j] \)
 */
predicate bool_clause_reif(array [int] of var bool: xs, array [int] of var bool: ys, var bool: b);
/** @group flatzinc.two Constrains \a m to be the maximum value in array \a xs. */
predicate array_int_maximum(var int: m, array [int] of var int: xs);
/** @group flatzinc.two Constrains \a m to be the maximum value in array \a xs. */
predicate array_float_maximum(var float: m, array [int] of var float: xs);
/** @group flatzinc.two Constrains \a m to be the minimum value in array \a xs. */
predicate array_int_minimum(var int: m, array [int] of var int: xs);
/** @group flatzinc.two Constrains \a m to be the minimum value in array \a xs. */
predicate array_float_minimum(var float: m, array [int] of var float: xs);

/***
  @groupdef flatzinc.twootwo FlatZinc builtins added in MiniZinc 2.0.2.

  These functions and predicates define built-in operations of the MiniZinc language
  that have been added in MiniZinc 2.0.2. Solvers that support these natively need
  to include a file called redefinitions-2.0.2.mzn in their solver library that
  redefines these predicates as builtins.

*/

/** @group flatzinc.twootwo Element constraint on array with MiniZinc index set, constrains \a xs[\a idx] = \a y
  This can be overridden in a solver that can perform the index calculation more efficiently than using
  a MiniZinc decomposition.
*/
predicate array_var_bool_element_nonshifted(var int: idx, array [int] of var bool: xs, var bool: y);
/** @group flatzinc.twootwo Element constraint on array with MiniZinc index set, constrains \a xs[\a idx] = \a y
  This can be overridden in a solver that can perform the index calculation more efficiently than using
  a MiniZinc decomposition.
*/
predicate array_var_int_element_nonshifted(var int: idx, array [int] of var int: xs, var int: y);
/** @group flatzinc.twootwo Element constraint on array with MiniZinc index set, constrains \a xs[\a idx] = \a y
  This can be overridden in a solver that can perform the index calculation more efficiently than using
  a MiniZinc decomposition.
*/
predicate array_var_float_element_nonshifted(
  var int: idx,
  array [int] of var float: xs,
  var float: y,
);
/** @group flatzinc.twootwo Element constraint on array with MiniZinc index set, constrains \a xs[\a idx] = \a y
  This can be overridden in a solver that can perform the index calculation more efficiently than using
  a MiniZinc decomposition.
*/
predicate array_var_set_element_nonshifted(
  var int: idx,
  array [int] of var set of int: xs,
  var set of int: y,
);

/***
  @groupdef flatzinc.twoone FlatZinc builtins added in MiniZinc 2.1.0.

  These functions and predicates define built-in operations of the MiniZinc language
  that have been added in MiniZinc 2.1.0. Solvers that support these natively need
  to include a file called redefinitions-2.1.0.mzn in their solver library that
  redefines these predicates as builtins.

*/

/** @group flatzinc.twoone Constrains \( \a x \in\ [ \a l, \a u ] \) */
predicate float_in(var float: x, float: l, float: u);
/** @group flatzinc.twoone Constrains the domain of \a x using the values in \a as, using each
  pair of values \a as[2*\p i-1]..\a as[2*\p i] for \p i in 1..\p n/2 as a possible range
 */
predicate float_dom(var float: x, array [int] of float: as);

/***
  @groupdef flatzinc.twooneone FlatZinc builtins added in MiniZinc 2.1.1.

  These functions and predicates define built-in operations of the MiniZinc language
  that have been added in MiniZinc 2.1.1. Solvers that support these natively need
  to include a file called redefinitions-2.1.1.mzn in their solver library that
  redefines these predicates as builtins.

*/

/** @group flatzinc.twooneone Returns variable constrained to be equal to the minimum of the set \a s.
  An alternative implementation can be found in the comments of the source code.
*/
function var $$E: min(var set of $$E: s);
/** @group flatzinc.twooneone Returns variable constrained to be equal to the maximum of the set \a s.
  An alternative implementation can be found in the comments of the source code.
*/
function var $$E: max(var set of $$E: s);

/***
  @groupdef flatzinc.twotwoone FlatZinc builtins added in MiniZinc 2.2.1.

  These functions and predicates define built-in operations of the MiniZinc language
  that have been added in MiniZinc 2.2.1. Solvers that support these natively need
  to include a file called redefinitions-2.2.1.mzn in their solver library that
  redefines these predicates as builtins.

*/

/** @group flatzinc.twotwoone Constrains \a z = \(\a x ^ {\a y}\), \a z is constrained to ``1 div pow(x, abs(y))`` when \(\a y < 0\) **/
predicate int_pow_fixed(var int: x, int: y, var int: z);

/***
  @groupdef flatzinc.twothreethree FlatZinc builtins added in MiniZinc 2.3.3.

  These functions and predicates define built-in operations of the MiniZinc language
  that have been added in MiniZinc 2.3.3. Solvers that support these natively need
  to include a file called redefinitions-2.3.3.mzn in their solver library that
  redefines these predicates as builtins.

*/

/** @group flatzinc.twothreethree Constrains \a x \( \in \) \a s */
predicate float_set_in(var float: x, set of float: s);

/***
  @groupdef flatzinc.twofivetwo FlatZinc builtins added in MiniZinc 2.5.2.

  These functions and predicates define built-in operations of the MiniZinc language
  that have been added in MiniZinc 2.5.2. Solvers that support these natively need
  to include a file called redefinitions-2.5.2.mzn in their solver library that
  redefines these predicates as builtins.

*/

/** @group flatzinc.twofivetwo Element constraint on 2d array with MiniZinc index set, constrains \a xs[\a idx1,\a idx2] = \a y
  This can be overridden in a solver that can perform the index calculation more efficiently than using
  a MiniZinc decomposition.
*/
predicate array_var_int_element2d_nonshifted(
  var int: idx1,
  var int: idx2,
  array [int, int] of var int: xs,
  var int: y,
);

/** @group flatzinc.twofivetwo Element constraint on 2d array with MiniZinc index set, constrains \a xs[\a idx1,\a idx2] = \a y
  This can be overridden in a solver that can perform the index calculation more efficiently than using
  a MiniZinc decomposition.
*/
predicate array_var_bool_element2d_nonshifted(
  var int: idx1,
  var int: idx2,
  array [int, int] of var bool: xs,
  var bool: y,
);

/** @group flatzinc.twofivetwo Element constraint on 2d array with MiniZinc index set, constrains \a xs[\a idx1,\a idx2] = \a y
  This can be overridden in a solver that can perform the index calculation more efficiently than using
  a MiniZinc decomposition.
*/
predicate array_var_float_element2d_nonshifted(
  var int: idx1,
  var int: idx2,
  array [int, int] of var float: xs,
  var float: y,
);

/** @group flatzinc.twofivetwo Element constraint on 2d array with MiniZinc index set, constrains \a xs[\a idx1,\a idx2] = \a y
  This can be overridden in a solver that can perform the index calculation more efficiently than using
  a MiniZinc decomposition.
*/
predicate array_var_set_element2d_nonshifted(
  var int: idx1,
  var int: idx2,
  array [int, int] of var set of int: xs,
  var set of int: y,
);

/***
  @groupdef flatzinc.twosevenone FlatZinc builtins added in MiniZinc 2.7.1.

  These functions and predicates define built-in operations of the MiniZinc language
  that have been added in MiniZinc 2.7.1. Solvers that support these natively need
  to include a file called redefinitions-2.7.1.mzn in their solver library that
  redefines these predicates as builtins.

*/

/** @group flatzinc.twosevenone Constrains \( \a y = \lceil{ \a x} \rceil \) */
predicate float_ceil(var float: x, var int: y);
/** @group flatzinc.twosevenone Constrains \( \a y = \lfloor{ \a x} \rfloor \) */
predicate float_floor(var float: x, var int: y);
/** @group flatzinc.twosevenone Constrains that \a y is the nearest integer to \a x */
predicate float_round(var float: x, var int: y);

/***
  @groupdef flatzinc.deprecated Deprecated FlatZinc builtins

	These builtin constraints are no longer produced by the MiniZinc compiler,
	and they will be removed in future versions of MiniZinc.

*/

/** @group flatzinc.deprecated Constrains \( \a b \leftrightarrow \bigvee_i \a xs[i]\)
*/
predicate array_bool_or(array [int] of var bool: xs, var bool: b)
  :: mzn_deprecated("2.7.0", "https://www.minizinc.org/doc-2.7.0/en/lib-flatzinc-deprecated.html");
