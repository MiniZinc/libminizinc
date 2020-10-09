include "all_different.mzn";

predicate fzn_inverse(array [int] of var int: x, array [int] of var int: y) =
  alldifferent(x) /\ alldifferent(y) /\
  forall (i in index_set(x), j in index_set(y)) (x[i] = j <-> y[j] = i);
