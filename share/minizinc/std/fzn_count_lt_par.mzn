include "fzn_count_lt.mzn";

predicate fzn_count_lt_par(array [int] of var int: xs, int: value, int: n) = fzn_count_lt(xs, value, n);

%-----------------------------------------------------------------------------%
