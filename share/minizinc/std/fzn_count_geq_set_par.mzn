include "fzn_count_geq_set.mzn";

predicate fzn_count_geq_set_par(array [int] of var set of int: xs, set of int: value, int: n) =
  fzn_count_geq_set(xs, value, n);

%-----------------------------------------------------------------------------%
