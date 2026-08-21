% The constraint `int_array_write`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `int_array_write` and the body
% below is dropped in favour of the propagator, or ship a file of this name with
% a decomposition of your own.

% See `fzn_inverse.mzn` for what the offset is: `index` names a position in the
% caller's own numbering, which the flat array cannot carry.

predicate fzn_int_array_write(
  list of var int: xs,
  int: offset,
  var int: index,
  var int: value,
  list of var int: result,
) =
  forall (j in index_set(xs)) (result[j] = if offset + j - 1 = index then value else xs[j] endif);
