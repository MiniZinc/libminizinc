predicate fzn_disjunctive_strict_opt_reif(
  array [int] of var opt int: start,
  array [int] of var int: duration,
  var bool: b,
) =
  b <->
    (
      forall (i in index_set(duration)) (duration[i] >= 0) /\
        forall (i, j in index_set(duration) where i < j) (
          absent(start[i]) \/
            absent(start[j]) \/
            deopt(start[i]) + duration[i] <= deopt(start[j]) \/
            deopt(start[j]) + duration[j] <= deopt(start[i])
        )
    );
