predicate fzn_disjunctive_strict(
  array [int] of var int: start,
  array [int] of var int: duration,
) = forall (i in index_set(duration)) (duration[i] >= 0) /\
  forall (
    i, j in index_set(duration) where i < j,
  ) (
    start[i] + duration[i] <= start[j] \/ start[j] + duration[j] <= start[i]
  );
