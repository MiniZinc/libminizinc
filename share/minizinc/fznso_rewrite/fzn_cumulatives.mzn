% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

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

include "fzn_int_cumulatives.mzn";

predicate fzn_cumulatives(
  array [int] of var int: start,
  array [int] of var int: duration,
  array [int] of var int: resource,
  array [int] of var int: machine,
  array [int] of var int: bound,
  bool: is_upper_bound,
  int: min_machine,
) =
  fzn_int_cumulatives(
    array1d(start),
    array1d(duration),
    array1d(resource),
    array1d(machine),
    array1d(bound),
    min_machine,
    is_upper_bound,
  );
