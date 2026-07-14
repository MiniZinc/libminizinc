include "all_disjoint.mzn";

predicate fzn_partition_set(array [int] of var set of int: xs, set of int: universe) =
  all_disjoint(xs) /\ universe == array_union (i in index_set(xs)) (xs[i]);
