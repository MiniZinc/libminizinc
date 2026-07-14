include "fzn_count_lt_float.mzn";

predicate fzn_count_lt_float_par(array [int] of var float: xs, float: value, int: n) =
  fzn_count_lt_float(xs, value, n);

%-----------------------------------------------------------------------------%
