predicate fzn_count_eq(array[int] of var int: x, var int: y, var int: c) =
    c = sum(i in index_set(x)) ( x[i] == y );

%-----------------------------------------------------------------------------%
