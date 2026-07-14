%-----------------------------------------------------------------------------%
% Requires that the array 'xs' is in increasing order (duplicates are allowed).
%-----------------------------------------------------------------------------%

predicate fzn_increasing_bool_reif(array [int] of var bool: xs, var bool: b) =
  b <-> forall (i in index_set(xs) diff {min(index_set(xs))}) (xs[i - 1] <= xs[i]);
