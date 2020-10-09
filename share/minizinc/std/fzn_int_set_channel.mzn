predicate fzn_int_set_channel(array[int] of var int: x,
                          array[int] of var set of int: y) =
    forall(i in index_set(x)) (x[i] in index_set(y)) /\
    forall(j in index_set(y)) (y[j] subset index_set(x)) /\
    forall(i in index_set(x), j in index_set(y)) (x[i] = j <-> i in y[j]);

%-----------------------------------------------------------------------------%
