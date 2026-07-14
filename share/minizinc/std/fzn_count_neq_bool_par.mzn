include "fzn_count_neq_bool.mzn";

predicate fzn_count_neq_bool_par(array [int] of var bool: xs, bool: value, int: n) =
  fzn_count_neq_bool(xs, value, n);

%-----------------------------------------------------------------------------%
