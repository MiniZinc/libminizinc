predicate fzn_count_eq_par(array[int] of var int: x, int: y, int: c) =
    c = sum(i in index_set(x)) ( bool2int(x[i] == y) );

%-----------------------------------------------------------------------------%
