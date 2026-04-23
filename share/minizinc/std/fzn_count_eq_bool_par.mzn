include "fzn_count_eq_bool.mzn";

predicate fzn_count_eq_bool_par(array [int] of var bool: x, bool: y, int: c) =
  fzn_count_eq_bool(x, y, c);

%-----------------------------------------------------------------------------%
