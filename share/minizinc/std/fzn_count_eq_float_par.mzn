include "fzn_count_eq_float.mzn";

predicate fzn_count_eq_float_par(array [int] of var float: x, float: y, int: c) =
  fzn_count_eq_float(x, y, c);

%-----------------------------------------------------------------------------%
