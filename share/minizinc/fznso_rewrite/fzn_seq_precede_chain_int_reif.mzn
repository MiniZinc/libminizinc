% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% The reified form of `fzn_seq_precede_chain_int.mzn`, under the registry's name.

include "fzn_int_seq_precede_chain_reif.mzn";

predicate fzn_seq_precede_chain_int_reif(array [int] of var int: xs, var bool: b) =
  fzn_int_seq_precede_chain_reif(array1d(xs), b);
