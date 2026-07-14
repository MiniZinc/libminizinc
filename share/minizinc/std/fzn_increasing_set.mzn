%-----------------------------------------------------------------------------%
% Requires that the array 'xs' is in increasing order (duplicates are allowed).
%-----------------------------------------------------------------------------%

predicate fzn_increasing_set(array [int] of var set of int: xs) =
  forall (i in index_set(xs) diff {min(index_set(xs))}) (xs[i - 1] <= xs[i]);
