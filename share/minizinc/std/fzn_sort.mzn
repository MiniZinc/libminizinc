include "alldifferent.mzn";
include "increasing.mzn";

predicate fzn_sort(array[int] of var int: x, array[int] of var int: y) =
    let {
        array[index_set(x)] of var index_set(y): p
    } in
        forall(i in index_set(x)) ( y[p[i]] == x[i] ) /\
        alldifferent(p) /\
        increasing(y);
