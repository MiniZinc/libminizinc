%-----------------------------------------------------------------------------%
% Constrains the array of objects 'x' to be all equal.
%-----------------------------------------------------------------------------%

predicate fzn_all_equal_set(array [int] of var set of int: xs) =
  forall (i, j in index_set(xs) where i < j) (xs[i] = xs[j]);
