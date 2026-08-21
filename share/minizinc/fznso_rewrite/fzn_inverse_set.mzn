% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% See `fzn_inverse.mzn` for what the offsets are and why a negative one is left
% to the decomposition.

include "fzn_set_of_int_inverse.mzn";

predicate fzn_inverse_set(
  array [int] of var set of int: f,
  array [int] of var set of int: invf,
) =
  if length(f) = 0 \/ length(invf) = 0 \/ min(index_set(f)) < 0 \/ min(index_set(invf)) < 0 then
    forall (i in index_set(f)) (f[i] subset index_set(invf)) /\
      forall (j in index_set(invf)) (invf[j] subset index_set(f)) /\
      forall (i in index_set(f), j in index_set(invf)) (j in f[i] <-> i in invf[j])
  else
    fzn_set_of_int_inverse(array1d(f), min(index_set(f)), array1d(invf), min(index_set(invf)))
  endif;
