% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% The reified form of `fzn_value_precede_chain_set.mzn`, under the registry's
% name.

include "fzn_set_of_int_value_precede_chain_reif.mzn";

predicate fzn_value_precede_chain_set_reif(
  array [int] of int: order,
  array [int] of var set of int: xs,
  var bool: b,
) = fzn_set_of_int_value_precede_chain_reif(array1d(order), array1d(xs), b);
