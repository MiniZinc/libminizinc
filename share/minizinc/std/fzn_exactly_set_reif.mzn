%-----------------------------------------------------------------------------%
% Requires exactly 'n' variables in 'x' to take the value 'v'.
%-----------------------------------------------------------------------------%

predicate fzn_exactly_set_reif(int: n, array[int] of var set of int: x, set of int: v, var bool: b) =
    b <-> n == sum(i in index_set(x)) ( bool2int(x[i] == v) );
