include "fzn_count_leq_set.mzn";

predicate fzn_count_leq_set_par(array [int] of var set of int: xs, set of int: value, int: n) =
  fzn_count_leq_set(xs, value, n);

%-----------------------------------------------------------------------------%
