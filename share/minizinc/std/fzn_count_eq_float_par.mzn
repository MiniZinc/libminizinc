include "fzn_count_eq_float.mzn";

predicate fzn_count_eq_float_par(array [int] of var float: xs, float: value, int: n) =
  fzn_count_eq_float(xs, value, n);

%-----------------------------------------------------------------------------%
