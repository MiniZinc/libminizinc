% The constraint `int_array_roots`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `int_array_roots` and the body
% below is dropped in favour of the propagator, or ship a file of this name with
% a decomposition of your own.

% See `fzn_range.mzn` for what the offset is and why a negative one is left to
% the decomposition. The registry puts the set a constraint determines last, so
% the image comes before the set of indices that map into it.

predicate fzn_int_array_roots(
  list of var int: xs,
  int: offset,
  var set of int: values,
  var set of int: s,
) =
  let {
    array [offset..offset + length(xs) - 1] of var int: x =
      array1d(offset..offset + length(xs) - 1, xs);
  } in forall (i in ub(s)) (i in s -> x[i] in values) /\
    forall (i in ub(values)) (
      i in values -> forall (j in index_set(x)) (x[j] = i -> j in s)
    );
