include "fzn_count_leq_bool.mzn";

predicate fzn_count_leq_bool_par(array [int] of var bool: xs, bool: value, int: n) =
  fzn_count_leq_bool(xs, value, n);

%-----------------------------------------------------------------------------%
