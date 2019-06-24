%-----------------------------------------------------------------------------%
% Requires that 'y' occurs in the array or set 'x'.
%-----------------------------------------------------------------------------%

predicate fzn_member_float_reif(array[int] of var float: x, var float: y, var bool: b) =
    b <-> exists(i in index_set(x)) ( x[i] == y );
