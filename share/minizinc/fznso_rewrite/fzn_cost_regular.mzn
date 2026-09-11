% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% A FlatZinc array is one-dimensional, so the transition matrix and the edge
% weights are passed flat, one state's row after another, and declared `list of`
% — one-based, as the type-checker checks. `cost_regular`'s alphabet is always
% `1..S`, so the offset the registry carries is one.

include "fzn_int_regular_cost.mzn";

predicate fzn_cost_regular(
  array [int] of var int: xs,
  int: states,
  int: alphabet,
  array [int, int] of int: transition,
  int: initial,
  set of int: accepting,
  array [int, int] of int: weight,
  var int: total_cost,
) =
  fzn_int_regular_cost(
    array1d(xs),
    states,
    alphabet,
    1,
    array1d(transition),
    initial,
    accepting,
    array1d(weight),
    total_cost,
  );
