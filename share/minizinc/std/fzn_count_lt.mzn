include "count_fn.mzn";

predicate fzn_count_lt(array[int] of var int: x, var int: y, var int: c) =
    count(x,y) > c;

%-----------------------------------------------------------------------------%
