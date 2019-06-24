predicate fzn_among_reif(var int: n, array[int] of var int: x, set of int: v, var bool: b) =
    b <-> ( n == sum(i in index_set(x)) ( bool2int(x[i] in v) ) );
