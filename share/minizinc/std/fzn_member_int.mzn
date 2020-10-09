%-----------------------------------------------------------------------------%
% Requires that 'y' occurs in the array or set 'x'.
%-----------------------------------------------------------------------------%

predicate fzn_member_int(array[int] of var int: x, var int: y) =
    exists(i in index_set(x)) ( x[i] == y );
