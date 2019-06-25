predicate fzn_sum_set_reif(array[int] of int: vs, array[int] of int: ws,
                           var set of int: x, var int: s, var bool: b) =
    b <-> s == sum(j in index_set(vs)) ( bool2int(j in x) * ws[j] );
