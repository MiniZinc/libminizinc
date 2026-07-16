predicate fzn_global_cardinality_low_up_set(
  array [int] of var set of int: xs,
  array [int] of int: cover,
  array [int] of int: lower_bound,
  array [int] of int: upper_bound,
) =
  forall (i in index_set(cover)) (
    let { var int: sd = count (v in xs) (cover[i] in v) } in lower_bound[i] <= sd /\ sd <= upper_bound[i]
  );
