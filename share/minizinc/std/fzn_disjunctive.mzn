predicate fzn_disjunctive(array [int] of var int: start, array [int] of var int: duration) =
  forall (i in index_set(duration)) (duration[i] >= 0) /\
    forall (i, j in index_set(duration) where i < j) (
      duration[i] = 0 \/ duration[j] = 0 \/ start[i] + duration[i] <= start[j] \/ start[j] + duration[j] <= start[i]
    );
