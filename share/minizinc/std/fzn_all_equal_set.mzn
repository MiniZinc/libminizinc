%-----------------------------------------------------------------------------%
% Constrains the array of objects 'x' to be all equal.
%-----------------------------------------------------------------------------%

predicate fzn_all_equal_set(array [int] of var set of int: x) =
  forall (i, j in index_set(x) where i < j) (x[i] = x[j]);
