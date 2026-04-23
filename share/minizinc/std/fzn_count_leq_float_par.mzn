include "fzn_count_leq_float.mzn";

predicate fzn_count_leq_float_par(array [int] of var float: x, float: y, int: c) =
  fzn_count_leq_float(x, y, c);

%-----------------------------------------------------------------------------%
