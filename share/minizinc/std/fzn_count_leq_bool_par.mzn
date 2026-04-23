include "fzn_count_leq_bool.mzn";

predicate fzn_count_leq_bool_par(array [int] of var bool: x, bool: y, int: c) =
  fzn_count_leq_bool(x, y, c);

%-----------------------------------------------------------------------------%
