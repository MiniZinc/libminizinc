%-----------------------------------------------------------------------------%
% Constrains the array of objects 'x' to be all equal.
%-----------------------------------------------------------------------------%

predicate fzn_all_equal_set_reif(array [int] of var set of int: xs, var bool: b) =
  b <-> forall (i, j in index_set(xs) where i < j) (xs[i] = xs[j]);
