% The constraint `float_array_arg_val`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `float_array_arg_val` and the
% body below is dropped in favour of the propagator, or ship a file of this name
% with a decomposition of your own.

% A FlatZinc array is one-based, so the index the caller numbers `xs` from is
% passed explicitly rather than left implicit in an `index_set` the solver never
% receives. `list of` is what states the one-basing, and what the type-checker
% holds the caller to.

include "fzn_bool_array_arg_max.mzn";
predicate fzn_float_array_arg_val(
  list of var float: xs,
  int: offset,
  var float: value,
  var int: index,
) =
  let {
    array [offset..offset + length(xs) - 1] of var float: x =
      array1d(offset..offset + length(xs) - 1, xs);
  } in (
    fzn_bool_array_arg_max([x[j] = value | j in index_set(x)], offset, index) /\ x[index] = value
  );
