include "global_cardinality.mzn";

predicate fzn_global_cardinality_low_up_closed_opt(
  array [int] of var opt int: xs,
  array [int] of int: cover,
  array [int] of int: lower_bound,
  array [int] of int: upper_bound,
) =
  forall (xi in xs) (absent(xi) \/ deopt(xi) in {d | d in cover}) /\
    global_cardinality(xs, cover, lower_bound, upper_bound);
