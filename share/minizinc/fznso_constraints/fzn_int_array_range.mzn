% The constraint `int_array_range`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `int_array_range` and the body
% below is dropped in favour of the propagator, or ship a file of this name with
% a decomposition of your own.

% See `fzn_inverse.mzn` for what the offset is and why a negative one is left to
% the decomposition.

predicate fzn_int_array_range(
  list of var int: xs,
  int: offset,
  var set of int: domain,
  var set of int: image,
) =
  let {
    array [offset..offset + length(xs) - 1] of var int: x =
      array1d(offset..offset + length(xs) - 1, xs);
  } in forall (i in ub(domain)) (i in domain -> x[i] in image) /\
    forall (i in ub(image)) (i in image -> exists (j in ub(domain)) (j in domain /\ x[j] = i));
