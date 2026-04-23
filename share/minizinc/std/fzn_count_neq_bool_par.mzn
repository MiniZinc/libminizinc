include "fzn_count_neq_bool.mzn";

predicate fzn_count_neq_bool_par(array [int] of var bool: x, bool: y, int: c) =
  fzn_count_neq_bool(x, y, c);

%-----------------------------------------------------------------------------%
