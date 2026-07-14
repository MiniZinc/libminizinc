%-----------------------------------------------------------------------------%
% Requires that the array 'x' is in increasing order (duplicates are allowed).
%-----------------------------------------------------------------------------%

predicate fzn_increasing_float_reif(array [int] of var float: xs, var bool: b) =
  b <-> forall (i in index_set(xs) diff {min(index_set(xs))}) (xs[i - 1] <= xs[i]);
