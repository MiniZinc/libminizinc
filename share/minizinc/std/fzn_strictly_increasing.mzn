%-----------------------------------------------------------------------------%
% Requires that the array 'x' is in strict increasing order
%-----------------------------------------------------------------------------%

predicate fzn_strictly_increasing_int(array[int] of var int: x) =
    forall(i in index_set(x) diff { min(index_set(x)) }) (x[i-1] < x[i]);

predicate fzn_strictly_increasing_bool(array[int] of var bool: x) =
    forall(i in index_set(x) diff { min(index_set(x)) }) (x[i-1] < x[i]);
