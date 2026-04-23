include "fzn_count_eq_bool_reif.mzn";

predicate fzn_count_eq_bool_par_reif(array [int] of var bool: x, bool: y, int: c, var bool: b) =
  fzn_count_eq_bool_reif(x, y, c, b);

%-----------------------------------------------------------------------------%
