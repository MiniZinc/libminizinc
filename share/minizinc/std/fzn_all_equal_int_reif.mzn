%-----------------------------------------------------------------------------%
% Constrains the array of objects 'xs' to be all equal.
%-----------------------------------------------------------------------------%

predicate fzn_all_equal_int_reif(array [int] of var int: xs, var bool: b) =
  b <-> forall (i, j in index_set(xs) where i < j) (xs[i] = xs[j]);
