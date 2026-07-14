include "alldifferent.mzn";
include "increasing.mzn";

predicate fzn_sort(array [int] of var int: xs, array [int] of var int: ys) =
  let {
    array [index_set(xs)] of var index_set(ys): p;
  } in forall (i in index_set(xs)) (ys[p[i]] == xs[i]) /\ alldifferent(p) /\ increasing(ys);
