include "fzn_count_eq_bool.mzn";

predicate fzn_count_eq_bool_par(array [int] of var bool: xs, bool: value, int: n) =
  fzn_count_eq_bool(xs, value, n);

%-----------------------------------------------------------------------------%
