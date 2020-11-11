include "fzn_count_eq.mzn";

predicate fzn_count_eq_par(array[int] of var int: x, int: y, int: c) =
    fzn_count_eq(x,y,c);

%-----------------------------------------------------------------------------%
