include "fzn_count_neq_set.mzn";

predicate fzn_count_neq_set_par(array [int] of var set of int: xs, set of int: value, int: n) =
  fzn_count_neq_set(xs, value, n);

%-----------------------------------------------------------------------------%
