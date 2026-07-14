%-----------------------------------------------------------------------------%
% Requires that the array 'x' is in strict increasing order
%-----------------------------------------------------------------------------%

predicate fzn_strictly_increasing_set(array [int] of var set of int: xs) =
  forall (i in index_set(xs) diff {min(index_set(xs))}) (xs[i - 1] < xs[i]);
