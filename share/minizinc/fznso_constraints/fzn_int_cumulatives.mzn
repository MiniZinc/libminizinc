% The constraint `int_cumulatives`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `int_cumulatives` and the body
% below is dropped in favour of the propagator, or ship a file of this name with
% a decomposition of your own.

% `bound` is indexed by machine number, which the caller numbers from
% `capacity_offset`; a FlatZinc array cannot carry an index set, so that number
% travels alongside `machine`, as it does for the other offset-taking
% predicates here. It is named `capacity` on the way in, because a `list of`
% and an `array [int] of` parameter are one type to the overload resolver: the
% two forms below can only be told apart by the position and name of what
% follows `machine`.
%
% Nothing here says the bounds must be fixed. A solver whose propagator only
% takes fixed bounds declares `bound` as a parameter array, and the compiler
% generates the dispatch that uses it when they are and this decomposition when
% they are not.

include "fzn_cumulatives_decomp.mzn";

predicate fzn_int_cumulatives(
  list of var int: start,
  list of var int: duration,
  list of var int: resource,
  list of var int: machine,
  list of var int: capacity,
  int: capacity_offset,
  bool: is_upper_bound,
) =
  fzn_cumulatives_decomp(
    start,
    duration,
    resource,
    machine,
    array1d(capacity_offset..capacity_offset + length(capacity) - 1, capacity),
    is_upper_bound,
  );
