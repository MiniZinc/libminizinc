% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% See `fzn_inverse.mzn` for what the offset is and why a negative one is left to
% the decomposition.

include "fzn_set_of_int_to_bool.mzn";

predicate fzn_link_set_to_booleans(var set of int: s, array [int] of var bool: flags) =
  if length(flags) = 0 \/ min(index_set(flags)) < 0 then
    forall (i in index_set(flags)) (flags[i] <-> i in s)
  else
    fzn_set_of_int_to_bool(s, array1d(flags), min(index_set(flags)))
  endif;
