include "fzn_count_neq_set.mzn";

predicate fzn_count_neq_set_par(array [int] of var set of int: x, set of int: y, int: c) =
  fzn_count_neq_set(x, y, c);

%-----------------------------------------------------------------------------%
