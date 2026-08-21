% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% See `fzn_range.mzn` for what the offset is and why a negative one is left to
% the decomposition. The registry puts the set a constraint determines last, so
% the image comes before the set of indices that map into it.

include "fzn_int_array_roots.mzn";

predicate fzn_roots(array [int] of var int: xs, var set of int: preimage, var set of int: image) =
  if length(xs) = 0 \/ min(index_set(xs)) < 0 then
    forall (i in ub(preimage)) (i in preimage -> xs[i] in image) /\
      forall (i in ub(image)) (
        i in image -> forall (j in index_set(xs)) (xs[j] = i -> j in preimage)
      )
  else
    fzn_int_array_roots(array1d(xs), min(index_set(xs)), image, preimage)
  endif;
