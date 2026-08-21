% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% `std` spells this `fzn_knapsack`; the registry names it for the type it
% takes and puts that first. The predicate a solver implements is the registry's;
% the overload `std` calls forwards to it.

include "fzn_int_knapsack.mzn";

predicate fzn_knapsack(
  array [int] of int: weight,
  array [int] of int: profit,
  array [int] of var int: xs,
  var int: total_weight,
  var int: total_profit,
) =
  fzn_int_knapsack(array1d(weight), array1d(profit), array1d(xs), total_weight, total_profit);
