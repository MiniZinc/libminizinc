% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% See `fzn_inverse.mzn`. This is the partial form: an index outside the other
% array's range is allowed, and only says nothing rather than failing.

include "fzn_int_inverse_in_range.mzn";
include "global_cardinality.mzn";

predicate fzn_inverse_in_range(array [int] of var int: f, array [int] of var int: invf) =
  if length(f) = 0 \/ length(invf) = 0 \/ min(index_set(f)) < 0 \/ min(index_set(invf)) < 0 then
    forall (i in index_set(f)) (f[i] in index_set(invf) -> invf[f[i]] = i) /\
      forall (j in index_set(invf)) (invf[j] in index_set(f) -> f[invf[j]] = j)
  else
    fzn_int_inverse_in_range(array1d(f), min(index_set(f)), array1d(invf), min(index_set(invf)))
  endif;
