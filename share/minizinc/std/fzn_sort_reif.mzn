include "sort_fn.mzn";

predicate fzn_sort_reif(array[int] of var int: x, array[int] of var int: y, var bool: b) =
    let { array[int] of var int: s = sort(x); } in b <-> s=y;
