include "increasing.mzn";

predicate fzn_increasing_bool(array[int] of var opt bool: x) =
    let {
        array [index_set(x)] of var bool: xx;
        constraint xx[min(index_set(x))] = x[min(index_set(x))] default lb_array(x);
        constraint forall (i in min(index_set(x)) + 1 .. max(index_set(x))) (xx[i] = x[i] default xx[i - 1]);
    } in increasing(xx);