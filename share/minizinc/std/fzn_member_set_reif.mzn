%-----------------------------------------------------------------------------%
% Requires that 'y' occurs in the array of set 'x'.
%-----------------------------------------------------------------------------%

predicate fzn_member_set_reif(array[int] of var set of int: x, var set of int: y, var bool: b) =
    b <-> exists(i in index_set(x)) ( x[i] == y );
