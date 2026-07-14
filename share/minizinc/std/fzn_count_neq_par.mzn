include "fzn_count_neq.mzn";

predicate fzn_count_neq_par(array [int] of var int: xs, int: value, int: n) =
  fzn_count_neq(xs, value, n);

%-----------------------------------------------------------------------------%
