include "increasing.mzn";

predicate fzn_increasing_bool(array [int] of var opt bool: xs) =
  let {
    array [index_set(xs)] of var bool: xx;
    constraint xx[min(index_set(xs))] = xs[min(index_set(xs))] default lb_array(xs);
    constraint
      forall (i in min(index_set(xs)) + 1..max(index_set(xs))) (xx[i] = xs[i] default xx[i - 1]);
  } in increasing(xx);
