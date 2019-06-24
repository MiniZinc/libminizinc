%-----------------------------------------------------------------------------%
% Requires at most 'n' variables in 'x' to take the value 'v'.
%-----------------------------------------------------------------------------%

predicate fzn_at_most_set_reif(int: n, array[int] of var set of int: x, set of int: v, var bool: b) =
    b <-> sum(i in index_set(x)) ( bool2int(x[i] == v) ) <= n;

