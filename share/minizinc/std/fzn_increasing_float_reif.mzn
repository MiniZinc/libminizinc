%-----------------------------------------------------------------------------%
% Requires that the array 'x' is in increasing order (duplicates are allowed).
%-----------------------------------------------------------------------------%

predicate fzn_increasing_float_reif(array [int] of var float: x, var bool: b) =
  b <-> forall (i in index_set(x) diff {min(index_set(x))}) (x[i - 1] <= x[i]);
