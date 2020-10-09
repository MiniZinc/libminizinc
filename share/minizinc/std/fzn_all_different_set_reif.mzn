%-----------------------------------------------------------------------------%
% Constrains the array of objects 'x' to be all different.
%-----------------------------------------------------------------------------%

predicate fzn_all_different_set_reif(array[int] of var set of int: x, var bool: b) =
    b <-> forall(i,j in index_set(x) where i < j) ( x[i] != x[j] );
