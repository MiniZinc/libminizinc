include "fzn_count_neq_float.mzn";

predicate fzn_count_neq_float_par(array [int] of var float: x, float: y, int: c) =
  fzn_count_neq_float(x, y, c);

%-----------------------------------------------------------------------------%
