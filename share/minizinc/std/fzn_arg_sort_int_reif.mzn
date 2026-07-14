include "all_different.mzn";

predicate fzn_arg_sort_int_reif(array [int] of var int: xs, array [int] of var int: permutation, var bool: b) =
  b <->
    (
      all_different(permutation) /\
        forall (j in 1..length(xs) - 1) (
          xs[permutation[j]] <= xs[permutation[j + 1]] /\ (xs[permutation[j]] == xs[permutation[j + 1]] -> permutation[j] < permutation[j + 1])
        )
    );
