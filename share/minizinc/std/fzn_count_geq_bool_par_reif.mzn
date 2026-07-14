include "fzn_count_geq_bool_reif.mzn";

predicate fzn_count_geq_bool_par_reif(array [int] of var bool: xs, bool: value, int: n, var bool: b) =
  fzn_count_geq_bool_reif(xs, value, n, b);

%-----------------------------------------------------------------------------%
