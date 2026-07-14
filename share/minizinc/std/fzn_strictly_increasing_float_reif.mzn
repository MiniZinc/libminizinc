%-----------------------------------------------------------------------------%
% Requires that the array 'xs' is in strict increasing order
%-----------------------------------------------------------------------------%

predicate fzn_strictly_increasing_float_reif(array [int] of var float: xs, var bool: b) =
  b <-> forall (i in index_set(xs) diff {min(index_set(xs))}) (xs[i - 1] < xs[i]);
