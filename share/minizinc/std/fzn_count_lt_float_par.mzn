include "fzn_count_lt_float.mzn";

predicate fzn_count_lt_float_par(array [int] of var float: x, float: y, int: c) =
  fzn_count_lt_float(x, y, c);

%-----------------------------------------------------------------------------%
