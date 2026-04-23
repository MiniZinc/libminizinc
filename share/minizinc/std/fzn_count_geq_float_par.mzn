include "fzn_count_geq_float.mzn";

predicate fzn_count_geq_float_par(array [int] of var float: x, float: y, int: c) =
  fzn_count_geq_float(x, y, c);

%-----------------------------------------------------------------------------%
