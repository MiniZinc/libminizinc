%-----------------------------------------------------------------------------%
% Requires that the array 'x' is in strict increasing order
%-----------------------------------------------------------------------------%

predicate fzn_strictly_increasing_int_reif(array[int] of var int: x, var bool: b) =
    b <-> forall(i in index_set(x) diff { min(index_set(x)) }) (x[i-1] < x[i]);
