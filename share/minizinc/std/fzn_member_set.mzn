%-----------------------------------------------------------------------------%
% Requires that 'y' occurs in the array of set 'x'.
%-----------------------------------------------------------------------------%

predicate fzn_member_set(array[int] of var set of int: x, var set of int: y) =
    exists(i in index_set(x)) ( x[i] == y );
