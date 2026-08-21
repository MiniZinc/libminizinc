% The constraint `int_array_writes_sequential`, as the FZnSO registry defines
% it.
%
% This file is a solver's override point: declare `int_array_writes_sequential`
% and the body below is dropped in favour of the propagator, or ship a file of
% this name with a decomposition of your own.

% See `fzn_writes.mzn`. Here the last write to a position wins, so the result is
% always determined and the constraint is functional.

include "arg_max.mzn";

predicate fzn_int_array_writes_sequential(
  list of var int: xs,
  int: offset,
  list of var int: index,
  list of var int: value,
  list of var int: result,
) =
  forall (i in index_set(xs)) (
    let {
      array [1..length(value) + 1] of var int: Vi =
        array1d(1..length(value) + 1, reverse(value) ++ [xs[i]]);
      array [1..length(value) + 1] of var bool: Pi =
        array1d(
          1..length(value) + 1,
          reverse([index[j] = offset + i - 1 | j in index_set(value)]) ++ [true],
        );
    } in result[i] = Vi[arg_max(Pi)]
  );
