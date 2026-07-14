include "fzn_count_leq_float.mzn";

predicate fzn_count_leq_float_par(array [int] of var float: xs, float: value, int: n) =
  fzn_count_leq_float(xs, value, n);

%-----------------------------------------------------------------------------%
