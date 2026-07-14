predicate fzn_disjunctive(array [int] of var int: starts, array [int] of var int: durations) =
  forall (i in index_set(durations)) (durations[i] >= 0) /\
    forall (i, j in index_set(durations) where i < j) (
      durations[i] = 0 \/ durations[j] = 0 \/ starts[i] + durations[i] <= starts[j] \/ starts[j] + durations[j] <= starts[i]
    );
