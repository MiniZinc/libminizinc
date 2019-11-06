include "count_fn.mzn";

predicate fzn_count_gt(array[int] of var int: x, var int: y, var int: c) =
    count(x,y) < c;

%-----------------------------------------------------------------------------%
