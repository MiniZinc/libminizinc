include "all_different.mzn";

predicate fzn_arg_sort_float(array [int] of var float: xs, array [int] of var int: permutation) =
  all_different(permutation) /\
    forall (j in 1..length(xs) - 1) (
      xs[permutation[j]] <= xs[permutation[j + 1]] /\ (xs[permutation[j]] == xs[permutation[j + 1]] -> permutation[j] < permutation[j + 1])
    );
