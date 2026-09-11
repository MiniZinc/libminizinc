% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% A FlatZinc array is one-dimensional, so the transition matrix is passed flat,
% one state's row after another, and declared `list of` — one-based, as the
% type-checker checks. The two-dimensional form the standard library's `regular`
% calls forwards to it.
%
% Unlike `std`, this does not set `fzn_regular_is_decomposed`: a solver may
% implement the plain form, and `fzn_regular_reif` then builds a reified
% automaton over it rather than decomposing.

include "fzn_int_regular.mzn";

predicate fzn_regular(
  array [int] of var int: xs,
  int: states,
  int: alphabet,
  array [int, int] of int: transition,
  int: initial,
  set of int: accepting,
) = fzn_int_regular(array1d(xs), states, alphabet, 1, array1d(transition), initial, accepting);
