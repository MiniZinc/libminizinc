include "alldifferent.mzn";
include "increasing.mzn";

predicate fzn_sort(array[int] of var int: x, array[int] of var int: y) =
    let {
        int: lx = min(index_set(x)),
        int: ux = max(index_set(x)),
        int: ly = min(index_set(y)),
        int: uy = max(index_set(y)),
        array[lx..ux] of var ly..uy: p
    } in
        forall(i in index_set(x)) ( y[p[i]] == x[i] ) /\
        alldifferent(p) /\
        increasing(y);
