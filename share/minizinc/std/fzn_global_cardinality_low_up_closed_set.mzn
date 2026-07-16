include "global_cardinality.mzn";

predicate fzn_global_cardinality_low_up_closed_set(
  array [int] of var set of int: xs,
  array [int] of int: cover,
  array [int] of int: lower_bound,
  array [int] of int: upper_bound,
) = global_cardinality(xs, cover, lower_bound, upper_bound) /\ forall (v in xs) (v subset {d | d in cover});
