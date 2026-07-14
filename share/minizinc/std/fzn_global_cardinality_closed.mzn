include "global_cardinality.mzn";

predicate fzn_global_cardinality_closed(
  array [int] of var int: xs,
  array [int] of int: cover,
  array [int] of var int: counts,
) = forall (i in index_set(xs)) (xs[i] in {d | d in cover}) /\ global_cardinality(xs, cover, counts);
