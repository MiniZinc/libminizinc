% The constraint `int_array_sort`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `int_array_sort` and the body
% below is dropped in favour of the propagator, or ship a file of this name with
% a decomposition of your own.

include "alldifferent.mzn";
include "increasing.mzn";

predicate fzn_int_array_sort(list of var int: xs, list of var int: ys) =
  let {
    array [index_set(xs)] of var index_set(ys): p;
  } in forall (i in index_set(xs)) (ys[p[i]] == xs[i]) /\ alldifferent(p) /\ increasing(ys);
