% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% `std` spells this `fzn_seq_precede_chain_set`; the registry names it for the type it
% takes and puts that first. The predicate a solver implements is the registry's;
% the overload `std` calls forwards to it.

include "fzn_set_of_int_seq_precede_chain.mzn";

predicate fzn_seq_precede_chain_set(array [int] of var set of int: xs) =
  fzn_set_of_int_seq_precede_chain(array1d(xs));
