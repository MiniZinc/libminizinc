include "fzn_count_lt_bool.mzn";

predicate fzn_count_lt_bool_par(array [int] of var bool: xs, bool: value, int: n) =
  fzn_count_lt_bool(xs, value, n);

%-----------------------------------------------------------------------------%
