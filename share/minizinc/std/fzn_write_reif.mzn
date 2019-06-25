predicate fzn_write_reif(array[int] of var int: I, var int: i, var int: v,
                    array[int] of var int: O, var bool: b) =
    b <-> forall(j in index_set(I))
                (O[j] = if j = i then v else I[j] endif);

%-----------------------------------------------------------------------------%
