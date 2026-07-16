predicate fzn_disjunctive_opt(array [int] of var opt int: start, array [int] of var int: duration) =
  forall (i, j in index_set(duration) where i < j) (
    absent(start[i]) \/
      absent(start[j]) \/
      duration[i] = 0 \/
      duration[j] = 0 \/
      deopt(start[i]) + duration[i] <= deopt(start[j]) \/
      deopt(start[j]) + duration[j] <= deopt(start[i])
  );
