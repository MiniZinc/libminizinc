include "global_cardinality.mzn";

predicate fzn_global_cardinality_closed_set(
  array [int] of var set of int: xs,
  array [int] of int: cover,
  array [int] of var int: count,
) = global_cardinality(xs, cover, count) /\ forall (v in xs) (v subset {d | d in cover});
