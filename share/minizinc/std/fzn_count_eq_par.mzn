include "fzn_count_eq.mzn";

predicate fzn_count_eq_par(array [int] of var int: xs, int: value, int: n) = fzn_count_eq(xs, value, n);

%-----------------------------------------------------------------------------%
