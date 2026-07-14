include "fzn_count_neq_float.mzn";

predicate fzn_count_neq_float_par(array [int] of var float: xs, float: value, int: n) =
  fzn_count_neq_float(xs, value, n);

%-----------------------------------------------------------------------------%
