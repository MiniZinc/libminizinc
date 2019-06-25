%-----------------------------------------------------------------------------%
% Constrains the array of objects 'x' to be all different.
%-----------------------------------------------------------------------------%

predicate fzn_all_different_int(array[int] of var int: x) =
    forall(i,j in index_set(x) where i < j) ( x[i] != x[j] );
