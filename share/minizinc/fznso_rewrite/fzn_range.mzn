% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% See `fzn_inverse.mzn` for what the offset is and why a negative one is left to
% the decomposition.

include "fzn_int_array_range.mzn";

predicate fzn_range(array [int] of var int: xs, var set of int: domain, var set of int: image) =
  if length(xs) = 0 \/ min(index_set(xs)) < 0 then
    forall (i in ub(domain)) (i in domain -> xs[i] in image) /\
      forall (i in ub(image)) (
        i in image -> exists (j in ub(domain)) (j in domain /\ xs[j] = i)
      )
  else
    fzn_int_array_range(array1d(xs), min(index_set(xs)), domain, image)
  endif;
