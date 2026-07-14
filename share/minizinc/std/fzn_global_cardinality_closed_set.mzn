include "global_cardinality.mzn";

predicate fzn_global_cardinality_closed_set(
  array [int] of var set of int: xs,
  array [int] of int: cover,
  array [int] of var int: counts,
) = global_cardinality(xs, cover, counts) /\ forall (v in xs) (v subset {d | d in cover});
