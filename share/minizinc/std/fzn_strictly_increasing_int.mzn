%-----------------------------------------------------------------------------%
% Requires that the array 'xs' is in strict increasing order
%-----------------------------------------------------------------------------%

predicate fzn_strictly_increasing_int(array [int] of var int: xs) =
  forall (i in index_set(xs) diff {min(index_set(xs))}) (xs[i - 1] < xs[i]);
