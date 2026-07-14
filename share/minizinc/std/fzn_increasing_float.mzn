%-----------------------------------------------------------------------------%
% Requires that the array 'x' is in increasing order (duplicates are allowed).
%-----------------------------------------------------------------------------%

predicate fzn_increasing_float(array [int] of var float: xs) =
  if length(xs) > 1 then
    forall (i in index_set(xs) diff {min(index_set(xs))}) (xs[i - 1] <= xs[i])
  endif;
