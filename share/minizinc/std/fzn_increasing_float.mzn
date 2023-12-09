%-----------------------------------------------------------------------------%
% Requires that the array 'x' is in increasing order (duplicates are allowed).
%-----------------------------------------------------------------------------%

predicate fzn_increasing_float(array[int] of var float: x) =
  if length(x) > 1 then
    forall(i in index_set(x) diff { min(index_set(x)) }) (x[i-1] <= x[i])
  endif;
