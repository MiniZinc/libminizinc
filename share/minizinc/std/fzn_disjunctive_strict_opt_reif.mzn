predicate fzn_disjunctive_strict_opt_reif(
  array [int] of var opt int: starts,
  array [int] of var int: durations,
  var bool: b,
) =
  b <->
    (
      forall (i in index_set(durations)) (durations[i] >= 0) /\
        forall (i, j in index_set(durations) where i < j) (
          absent(starts[i]) \/
            absent(starts[j]) \/
            deopt(starts[i]) + durations[i] <= deopt(starts[j]) \/
            deopt(starts[j]) + durations[j] <= deopt(starts[i])
        )
    );
