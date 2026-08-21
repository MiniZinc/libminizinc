% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% `std` spells this `fzn_mdd`; the registry names it for the type it
% takes and puts that first. The predicate a solver implements is the registry's;
% the overload `std` calls forwards to it.

include "fzn_int_mdd.mzn";

predicate fzn_mdd(
  array [int] of var int: xs,
  int: nodes,
  array [int] of int: level,
  int: edges,
  array [int] of int: from,
  array [int] of set of int: label,
  array [int] of int: to,
) =
  fzn_int_mdd(array1d(xs), nodes, array1d(level), edges, array1d(from), array1d(label), array1d(to));
