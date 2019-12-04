include "count_fn.mzn";

predicate fzn_count_neq_par(array[int] of var int: x, int: y, int: c) =
    count(x,y) != c;

%-----------------------------------------------------------------------------%
