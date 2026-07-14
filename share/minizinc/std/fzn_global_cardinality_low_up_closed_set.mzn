include "global_cardinality.mzn";

predicate fzn_global_cardinality_low_up_closed_set(
  array [int] of var set of int: xs,
  array [int] of int: cover,
  array [int] of int: lbound,
  array [int] of int: ubound,
) = global_cardinality(xs, cover, lbound, ubound) /\ forall (v in xs) (v subset {d | d in cover});
