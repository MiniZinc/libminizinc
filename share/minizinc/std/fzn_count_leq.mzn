predicate fzn_count_leq(array[int] of var int: x, var int: y, var int: c) =
    c <= sum(i in index_set(x)) ( bool2int(x[i] == y) );

%-----------------------------------------------------------------------------%
