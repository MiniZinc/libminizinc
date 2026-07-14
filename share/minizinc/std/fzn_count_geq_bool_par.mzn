include "fzn_count_geq_bool.mzn";

predicate fzn_count_geq_bool_par(array [int] of var bool: xs, bool: value, int: n) =
  fzn_count_geq_bool(xs, value, n);

%-----------------------------------------------------------------------------%
