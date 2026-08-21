% FlatZinc builtins, rewritten onto the names the FZnSO registry gives them.
%
% MiniZinc emits a builtin whatever a solver declares, so every one of them is
% rewritten here — there is no builtin a solver sees under its own name. Where
% the registry names the constraint outright the rewrite is a rename; where it
% names it through the linear and clause constraints instead, the body says the
% same thing in that vocabulary. Either way it is an identity, not an
% approximation, and a solver that declares the builtin keeps it:
% `enable_native_predicates` drops the body before any of this is reached.
%
% Everything lands on an `fzn_<ident>` predicate in `fznso_constraints/`, which
% is a solver's override point. A solver behind this interface ships no
% `redefinitions*.mzn` of its own — those belong to this layer, and two
% libraries defining one builtin is an ambiguity rather than an override.
%
% Every body names an `fzn_` predicate explicitly. Writing `x = y` would flatten
% back to `int_eq`, and that is the redefinition being defined.

include "fzn_bool_array_xor_reif.mzn";
include "fzn_bool_array_and.mzn";
include "fzn_bool_array_element.mzn";
include "fzn_bool_array_xor.mzn";
include "fzn_bool_clause.mzn";
include "fzn_bool_clause_reif.mzn";
include "fzn_bool_lin_eq.mzn";
include "fzn_bool_lin_le.mzn";
include "fzn_bool_lin_le_reif.mzn";
include "fzn_bool_lin_ne.mzn";
include "fzn_bool_lin_ne_reif.mzn";
include "fzn_bool_to_int.mzn";
include "fzn_float_abs.mzn";
include "fzn_float_acos.mzn";
include "fzn_float_acosh.mzn";
include "fzn_float_array_element.mzn";
include "fzn_float_asin.mzn";
include "fzn_float_asinh.mzn";
include "fzn_float_atan.mzn";
include "fzn_float_atanh.mzn";
include "fzn_float_cos.mzn";
include "fzn_float_cosh.mzn";
include "fzn_float_div.mzn";
include "fzn_float_exp.mzn";
include "fzn_float_in_reif.mzn";
include "fzn_float_lin_eq.mzn";
include "fzn_float_lin_eq_reif.mzn";
include "fzn_float_lin_le.mzn";
include "fzn_float_lin_le_reif.mzn";
include "fzn_float_lin_lt.mzn";
include "fzn_float_lin_lt_reif.mzn";
include "fzn_float_lin_ne.mzn";
include "fzn_float_lin_ne_reif.mzn";
include "fzn_float_ln.mzn";
include "fzn_float_log10.mzn";
include "fzn_float_log2.mzn";
include "fzn_float_pow.mzn";
include "fzn_float_sin.mzn";
include "fzn_float_sinh.mzn";
include "fzn_float_sqrt.mzn";
include "fzn_float_tan.mzn";
include "fzn_float_tanh.mzn";
include "fzn_float_times.mzn";
include "fzn_int_abs.mzn";
include "fzn_int_array_element.mzn";
include "fzn_int_div.mzn";
include "fzn_int_in.mzn";
include "fzn_int_in_reif.mzn";
include "fzn_int_lin_eq.mzn";
include "fzn_int_lin_eq_reif.mzn";
include "fzn_int_lin_le.mzn";
include "fzn_int_lin_le_reif.mzn";
include "fzn_int_lin_ne.mzn";
include "fzn_int_lin_ne_reif.mzn";
include "fzn_int_mod.mzn";
include "fzn_int_pow.mzn";
include "fzn_int_times.mzn";
include "fzn_int_to_float.mzn";
include "fzn_set_of_int_array_element.mzn";
include "fzn_set_of_int_card.mzn";
include "fzn_set_of_int_diff.mzn";
include "fzn_set_of_int_eq.mzn";
include "fzn_set_of_int_eq_reif.mzn";
include "fzn_set_of_int_intersect.mzn";
include "fzn_set_of_int_le.mzn";
include "fzn_set_of_int_le_reif.mzn";
include "fzn_set_of_int_lt.mzn";
include "fzn_set_of_int_lt_reif.mzn";
include "fzn_set_of_int_ne.mzn";
include "fzn_set_of_int_ne_reif.mzn";
include "fzn_set_of_int_subset.mzn";
include "fzn_set_of_int_subset_reif.mzn";
include "fzn_set_of_int_symdiff.mzn";
include "fzn_set_of_int_union.mzn";
include "fzn_float_array_maximum.mzn";
include "fzn_float_array_minimum.mzn";
include "fzn_int_array_maximum.mzn";
include "fzn_int_array_minimum.mzn";

% --- element -----------------------------------------------------------------
% Reading a value out of an array by a variable index. The `_nonshifted`
% forms in `redefinitions-2.0.2.mzn` carry the caller's own numbering, which
% is what the registry's `offset` argument is for; these are one-based by
% definition, so the offset they pass is 1.

predicate array_bool_element(var int: idx, array [int] of bool: xs, var bool: y) =
  fzn_bool_array_element(array1d(xs), 1, idx, y);

predicate array_float_element(var int: idx, array [int] of float: xs, var float: y) =
  fzn_float_array_element(array1d(xs), 1, idx, y);

predicate array_int_element(var int: idx, array [int] of int: xs, var int: y) =
  fzn_int_array_element(array1d(xs), 1, idx, y);

predicate array_set_element(var int: idx, array [int] of set of int: xs, var set of int: y) =
  fzn_set_of_int_array_element(array1d(xs), 1, idx, y);

predicate array_var_bool_element(var int: idx, array [int] of var bool: xs, var bool: y) =
  fzn_bool_array_element(array1d(xs), 1, idx, y);

predicate array_var_float_element(var int: idx, array [int] of var float: xs, var float: y) =
  fzn_float_array_element(array1d(xs), 1, idx, y);

predicate array_var_int_element(var int: idx, array [int] of var int: xs, var int: y) =
  fzn_int_array_element(array1d(xs), 1, idx, y);

predicate array_var_set_element(var int: idx, array [int] of var set of int: xs, var set of int: y) =
  fzn_set_of_int_array_element(array1d(xs), 1, idx, y);

% --- extrema -----------------------------------------------------------------
% The registry puts the value a constraint determines last, and names the
% array form only — an extremum of two is that form with an array of two.

predicate float_max(var float: x, var float: y, var float: z) =
  fzn_float_array_maximum([x, y], z);

predicate float_min(var float: x, var float: y, var float: z) =
  fzn_float_array_minimum([x, y], z);

predicate int_max(var int: x, var int: y, var int: z) = fzn_int_array_maximum([x, y], z);

predicate int_min(var int: x, var int: y, var int: z) = fzn_int_array_minimum([x, y], z);

% --- booleans ----------------------------------------------------------------
% A clause is `⋁pos ∨ ⋁¬neg`, so an implication is a clause of one each way
% and an equality is the two implications. `bool_array_xor` says an odd
% number of its arguments hold, which is what makes it the reified equality, its
% negation the reified exclusive or, and the pair `[x, y]` the negation of one
% Boolean by the other. The registry has no `bool_array_or`:
% `r ↔ ⋁xs` is exactly `bool_clause_reif(xs, [], r)`, which is what a SAT solver
% encodes an or-gate as anyway.

predicate array_bool_and(array [int] of var bool: xs, var bool: b) =
  fzn_bool_array_and(array1d(xs), b);

predicate array_bool_or(array [int] of var bool: xs, var bool: b) =
  fzn_bool_clause_reif(array1d(xs), [], b);

predicate array_bool_xor(array [int] of var bool: xs) = fzn_bool_array_xor(array1d(xs));

predicate array_bool_xor_reif(array [int] of var bool: xs, var bool: b) =
  fzn_bool_array_xor_reif(array1d(xs), b);

predicate bool2int(var bool: x, var int: y) = fzn_bool_to_int(x, y);

predicate bool_and(var bool: x, var bool: y, var bool: b) = fzn_bool_array_and([x, y], b);

predicate bool_clause(
  array [int] of var bool: xs :: promise_ctx_monotone,
  array [int] of var bool: ys :: promise_ctx_antitone,
) =
  fzn_bool_clause(array1d(xs), array1d(ys));

predicate bool_clause_reif(
  array [int] of var bool: xs :: promise_ctx_monotone,
  array [int] of var bool: ys :: promise_ctx_antitone,
  var bool: b,
) =
  fzn_bool_clause_reif(array1d(xs), array1d(ys), b);

% `bool_eq` is deliberately not rewritten. An equality between two Booleans is
% an alias, and the compiler resolves it.

predicate bool_eq_reif(var bool: x, var bool: y, var bool: b) =
  fzn_bool_array_xor([x, y, b]);

predicate bool_le(
  var bool: x :: promise_ctx_antitone,
  var bool: y :: promise_ctx_monotone,
) = fzn_bool_clause([y], [x]);

predicate bool_le_reif(var bool: x, var bool: y, var bool: b) =
  fzn_bool_clause_reif([y], [x], b);

predicate bool_lin_eq(array [int] of int: as, array [int] of var bool: xs, var int: y) =
  fzn_bool_lin_eq(array1d(as), array1d(xs), y);

predicate bool_lin_le(array [int] of int: as, array [int] of var bool: xs, int: y) =
  fzn_bool_lin_le(array1d(as), array1d(xs), y);

predicate bool_lin_le_reif(array [int] of int: as, array [int] of var bool: xs, int: y, var bool: b) =
  fzn_bool_lin_le_reif(array1d(as), array1d(xs), y, b);

predicate bool_lin_ne(array [int] of int: as, array [int] of var bool: xs, var int: y) =
  fzn_bool_lin_ne(array1d(as), array1d(xs), y);

predicate bool_lin_ne_reif(
  array [int] of int: as,
  array [int] of var bool: xs,
  var int: y,
  var bool: b,
) = fzn_bool_lin_ne_reif(array1d(as), array1d(xs), y, b);

predicate bool_lt(
  var bool: x :: promise_ctx_antitone,
  var bool: y :: promise_ctx_monotone,
) = fzn_bool_clause([], [x]) /\ fzn_bool_clause([y], []);

predicate bool_lt_reif(var bool: x, var bool: y, var bool: b) =
  fzn_bool_array_and([not x, y], b);

% A constant operand is folded here rather than left in the array. `not true`
% is a parameter expression, so the flattener binds the other side outright and
% no constraint is posted at all; buried in an array literal the same constant
% is invisible to it, and `o_bool_array_xor` cannot recover it later because a
% constraint no variable event touches is never put on the optimiser's queue.
predicate bool_not(var bool: x, var bool: y) =
  if is_fixed(x) then
    y = not fix(x)
  elseif is_fixed(y) then
    x = not fix(y)
  else
    fzn_bool_array_xor([x, y])
  endif;

predicate bool_or(var bool: x, var bool: y, var bool: b) = fzn_bool_clause_reif([x, y], [], b);

predicate bool_xor(var bool: x, var bool: y, var bool: b) =
  fzn_bool_array_xor([x, y, not b]);

predicate bool_xor_reif(var bool: x, var bool: y, var bool: b, var bool: r) =
  fzn_bool_array_xor_reif([x, y, not b], r);

% --- integers ----------------------------------------------------------------

predicate int2float(var int: x, var float: y) = fzn_int_to_float(x, y);

predicate int_abs(var int: x, var int: y) =
  fzn_int_abs(x, y);

predicate int_div(var int: x, var int: y, var int: z) =
  fzn_int_div(x, y, z);

predicate int_eq(var int: x, var int: y) = fzn_int_lin_eq([1, -1], [x, y], 0);

predicate int_eq_reif(var int: x, var int: y, var bool: b) =
  fzn_int_lin_eq_reif([1, -1], [x, y], 0, b);

predicate int_le(var int: x, var int: y) = fzn_int_lin_le([1, -1], [x, y], 0);

predicate int_le_reif(var int: x, var int: y, var bool: b) =
  fzn_int_lin_le_reif([1, -1], [x, y], 0, b);

predicate int_lin_eq(array [int] of int: as, array [int] of var int: xs, int: y) =
  fzn_int_lin_eq(array1d(as), array1d(xs), y);

predicate int_lin_eq_reif(
  array [int] of int: as,
  array [int] of var int: xs,
  int: y,
  var bool: b,
) =
  fzn_int_lin_eq_reif(array1d(as), array1d(xs), y, b);

predicate int_lin_le(array [int] of int: as, array [int] of var int: xs, int: y) =
  fzn_int_lin_le(array1d(as), array1d(xs), y);

predicate int_lin_le_reif(
  array [int] of int: as,
  array [int] of var int: xs,
  int: y,
  var bool: b,
) =
  fzn_int_lin_le_reif(array1d(as), array1d(xs), y, b);

predicate int_lin_ne(array [int] of int: as, array [int] of var int: xs, int: y) =
  fzn_int_lin_ne(array1d(as), array1d(xs), y);

predicate int_lin_ne_reif(
  array [int] of int: as,
  array [int] of var int: xs,
  int: y,
  var bool: b,
) =
  fzn_int_lin_ne_reif(array1d(as), array1d(xs), y, b);

predicate int_lt(var int: x, var int: y) = fzn_int_lin_le([1, -1], [x, y], -1);

predicate int_lt_reif(var int: x, var int: y, var bool: b) =
  fzn_int_lin_le_reif([1, -1], [x, y], -1, b);

predicate int_mod(var int: x, var int: y, var int: z) =
  fzn_int_mod(x, y, z);

predicate int_ne(var int: x, var int: y) = fzn_int_lin_ne([1, -1], [x, y], 0);

predicate int_ne_reif(var int: x, var int: y, var bool: b) =
  fzn_int_lin_ne_reif([1, -1], [x, y], 0, b);

predicate int_plus(var int: x, var int: y, var int: z) =
  fzn_int_lin_eq([1, 1, -1], [x, y, z], 0);

predicate int_plus_reif(var int: x, var int: y, var int: z, var bool: b) =
  fzn_int_lin_eq_reif([1, 1, -1], [x, y, z], 0, b);

predicate int_pow(var int: x, var int: y, var int: z) = fzn_int_pow(x, y, z);

predicate int_times(var int: x, var int: y, var int: z) =
  fzn_int_times(x, y, z);

% --- floats ------------------------------------------------------------------
% As the integers, except that floats have a strict linear constraint of
% their own, so `<` does not become `<=` against a shifted bound.

predicate float_abs(var float: x, var float: y) =
  fzn_float_abs(x, y);

predicate float_acos(var float: x, var float: y) =
  fzn_float_acos(x, y);

predicate float_acosh(var float: x, var float: y) =
  fzn_float_acosh(x, y);

predicate float_asin(var float: x, var float: y) =
  fzn_float_asin(x, y);

predicate float_asinh(var float: x, var float: y) =
  fzn_float_asinh(x, y);

predicate float_atan(var float: x, var float: y) =
  fzn_float_atan(x, y);

predicate float_atanh(var float: x, var float: y) =
  fzn_float_atanh(x, y);

predicate float_cos(var float: x, var float: y) =
  fzn_float_cos(x, y);

predicate float_cosh(var float: x, var float: y) = fzn_float_cosh(x, y);

predicate float_div(var float: x, var float: y, var float: z) =
  fzn_float_div(x, y, z);

predicate float_eq(var float: x, var float: y) = fzn_float_lin_eq([1.0, -1.0], [x, y], 0.0);

predicate float_eq_reif(var float: x, var float: y, var bool: b) =
  fzn_float_lin_eq_reif([1.0, -1.0], [x, y], 0.0, b);

predicate float_exp(var float: x, var float: y) =
  fzn_float_exp(x, y);

predicate float_in_reif(var float: x, float: l, float: u, var bool: b) =
  fzn_float_in_reif(x, l, u, b);

predicate float_le(var float: x, var float: y) = fzn_float_lin_le([1.0, -1.0], [x, y], 0.0);

predicate float_le_reif(var float: x, var float: y, var bool: b) =
  fzn_float_lin_le_reif([1.0, -1.0], [x, y], 0.0, b);

predicate float_lin_eq(array [int] of float: as, array [int] of var float: xs, float: y) =
  fzn_float_lin_eq(array1d(as), array1d(xs), y);

predicate float_lin_eq_reif(
  array [int] of float: as,
  array [int] of var float: xs,
  float: y,
  var bool: b,
) =
  fzn_float_lin_eq_reif(array1d(as), array1d(xs), y, b);

predicate float_lin_le(array [int] of float: as, array [int] of var float: xs, float: y) =
  fzn_float_lin_le(array1d(as), array1d(xs), y);

predicate float_lin_le_reif(
  array [int] of float: as,
  array [int] of var float: xs,
  float: y,
  var bool: b,
) =
  fzn_float_lin_le_reif(array1d(as), array1d(xs), y, b);

predicate float_lin_lt(array [int] of float: as, array [int] of var float: xs, float: y) =
  fzn_float_lin_lt(array1d(as), array1d(xs), y);

predicate float_lin_lt_reif(
  array [int] of float: as,
  array [int] of var float: xs,
  float: y,
  var bool: b,
) =
  fzn_float_lin_lt_reif(array1d(as), array1d(xs), y, b);

predicate float_lin_ne(array [int] of float: as, array [int] of var float: xs, float: y) =
  fzn_float_lin_ne(array1d(as), array1d(xs), y);

predicate float_lin_ne_reif(
  array [int] of float: as,
  array [int] of var float: xs,
  float: y,
  var bool: b,
) =
  fzn_float_lin_ne_reif(array1d(as), array1d(xs), y, b);

predicate float_ln(var float: x, var float: y) =
  fzn_float_ln(x, y);

predicate float_log10(var float: x, var float: y) =
  fzn_float_log10(x, y);

predicate float_log2(var float: x, var float: y) =
  fzn_float_log2(x, y);

predicate float_lt(var float: x, var float: y) = fzn_float_lin_lt([1.0, -1.0], [x, y], 0.0);

predicate float_lt_reif(var float: x, var float: y, var bool: b) =
  fzn_float_lin_lt_reif([1.0, -1.0], [x, y], 0.0, b);

predicate float_ne(var float: x, var float: y) = fzn_float_lin_ne([1.0, -1.0], [x, y], 0.0);

predicate float_ne_reif(var float: x, var float: y, var bool: b) =
  fzn_float_lin_ne_reif([1.0, -1.0], [x, y], 0.0, b);

predicate float_plus(var float: x, var float: y, var float: z) =
  fzn_float_lin_eq([1.0, 1.0, -1.0], [x, y, z], 0.0);

predicate float_plus_reif(var float: x, var float: y, var float: z, var bool: b) =
  fzn_float_lin_eq_reif([1.0, 1.0, -1.0], [x, y, z], 0.0, b);

predicate float_pow(var float: x, var float: y, var float: z) =
  fzn_float_pow(x, y, z);

predicate float_sin(var float: x, var float: y) =
  fzn_float_sin(x, y);

predicate float_sinh(var float: x, var float: y) = fzn_float_sinh(x, y);

predicate float_sqrt(var float: x, var float: y) =
  fzn_float_sqrt(x, y);

predicate float_tan(var float: x, var float: y) =
  fzn_float_tan(x, y);

predicate float_tanh(var float: x, var float: y) = fzn_float_tanh(x, y);

predicate float_times(var float: x, var float: y, var float: z) =
  fzn_float_times(x, y, z);

% --- sets --------------------------------------------------------------------
% `set_superset` has no entry of its own: it is `set_of_int_subset` with its
% arguments the other way round.

predicate set_card(var set of int: s, var int: x) =
  fzn_set_of_int_card(s, x);

predicate set_diff(var set of int: s, var set of int: t, var set of int: r) =
  fzn_set_of_int_diff(s, t, r);

predicate set_eq(var set of int: s, var set of int: t) =
  fzn_set_of_int_eq(s, t);

predicate set_eq_reif(var set of int: s, var set of int: t, var bool: b) =
  fzn_set_of_int_eq_reif(s, t, b);

predicate set_in(var int: x, set of int: s) =
  fzn_int_in(x, s);

predicate set_in(var int: x, var set of int: s) =
  fzn_int_in(x, s);

predicate set_in_reif(var int: x, set of int: s, var bool: b) =
  fzn_int_in_reif(x, s, b);

predicate set_in_reif(var int: x, var set of int: s, var bool: b) =
  fzn_int_in_reif(x, s, b);

predicate set_intersect(var set of int: s, var set of int: t, var set of int: r) =
  fzn_set_of_int_intersect(s, t, r);

predicate set_le(var set of int: s, var set of int: t) =
  fzn_set_of_int_le(s, t);

predicate set_le_reif(var set of int: s, var set of int: t, var bool: b) =
  fzn_set_of_int_le_reif(s, t, b);

predicate set_lt(var set of int: s, var set of int: t) =
  fzn_set_of_int_lt(s, t);

predicate set_lt_reif(var set of int: s, var set of int: t, var bool: b) =
  fzn_set_of_int_lt_reif(s, t, b);

predicate set_ne(var set of int: s, var set of int: t) =
  fzn_set_of_int_ne(s, t);

predicate set_ne_reif(var set of int: s, var set of int: t, var bool: b) =
  fzn_set_of_int_ne_reif(s, t, b);

predicate set_subset(var set of int: s, var set of int: t) =
  fzn_set_of_int_subset(s, t);

predicate set_subset_reif(var set of int: s, var set of int: t, var bool: b) =
  fzn_set_of_int_subset_reif(s, t, b);

predicate set_superset(var set of int: s, var set of int: t) =
  fzn_set_of_int_subset(t, s);

predicate set_superset_reif(var set of int: s, var set of int: t, var bool: b) =
  fzn_set_of_int_subset_reif(t, s, b);

predicate set_symdiff(var set of int: s, var set of int: t, var set of int: r) =
  fzn_set_of_int_symdiff(s, t, r);

predicate set_union(var set of int: s, var set of int: t, var set of int: r) =
  fzn_set_of_int_union(s, t, r);
