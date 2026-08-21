% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% The reified form of `fzn_sliding_among_bool.mzn`, under the registry's name.

include "fzn_bool_sliding_among_reif.mzn";
include "sliding_sum.mzn";

predicate fzn_sliding_among_bool_reif(
  int: lower_bound,
  int: upper_bound,
  int: window,
  array [int] of var bool: xs,
  bool: value,
  var bool: b,
) =
  fzn_bool_sliding_among_reif(array1d(xs), value, window, lower_bound, upper_bound, b);
