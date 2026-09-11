% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% The reified form of `fzn_alldifferent_except.mzn`, under the registry's name.

include "fzn_int_all_different_except_reif.mzn";
include "global_cardinality_low_up.mzn";

predicate fzn_alldifferent_except_reif(
  array [int] of var int: xs,
  set of int: except,
  var bool: b,
) =
  fzn_int_all_different_except_reif(array1d(xs), except, b);
