%-----------------------------------------------------------------------------%
% Requires that 'y' occurs in the array or set 'x'.
%-----------------------------------------------------------------------------%

predicate fzn_set_member_reif(var set of int: x, var int: y, var bool: b) =
    b <-> y in x;
