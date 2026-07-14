include "fzn_count_leq.mzn";

predicate fzn_count_leq_par(array [int] of var int: xs, int: value, int: n) = fzn_count_leq(xs, value, n);

%-----------------------------------------------------------------------------%
