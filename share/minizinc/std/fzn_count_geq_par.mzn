include "fzn_count_geq.mzn";

predicate fzn_count_geq_par(array [int] of var int: xs, int: value, int: n) =
  fzn_count_geq(xs, value, n);

%-----------------------------------------------------------------------------%
