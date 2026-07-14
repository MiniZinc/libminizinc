%-----------------------------------------------------------------------------%
% Constrains the array of objects 'x' to be all different.
%-----------------------------------------------------------------------------%

predicate fzn_all_different_int(array [int] of var int: xs) =
  forall (i, j in index_set(xs) where i < j) (xs[i] != xs[j]);
