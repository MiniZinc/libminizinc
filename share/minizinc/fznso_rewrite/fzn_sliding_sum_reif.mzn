% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% The reified form of `fzn_sliding_sum.mzn`, under the registry's name.

include "fzn_int_sliding_sum_reif.mzn";

predicate fzn_sliding_sum_reif(
  int: lower_bound,
  int: upper_bound,
  int: window,
  array [int] of var int: xs,
  var bool: b,
) =
  fzn_int_sliding_sum_reif(array1d(xs), window, lower_bound, upper_bound, b);
