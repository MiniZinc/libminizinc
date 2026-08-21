% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% `std` spells this `fzn_partition_set`; the registry names it for the type it
% takes and puts that first. The predicate a solver implements is the registry's;
% the overload `std` calls forwards to it.

include "fzn_set_of_int_partition.mzn";
include "all_disjoint.mzn";

predicate fzn_partition_set(array [int] of var set of int: xs, set of int: universe) =
  fzn_set_of_int_partition(array1d(xs), universe);
