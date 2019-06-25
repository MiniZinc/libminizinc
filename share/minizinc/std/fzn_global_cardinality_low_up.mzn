predicate fzn_global_cardinality_low_up(array[int] of var int: x,
                                    array[int] of int: cover,
                                    array[int] of int: lbound,
                                    array[int] of int: ubound) =
     forall(i in index_set(cover)) (
         sum(j in index_set(x)) ( bool2int(x[j] = cover[i]) )
         in lbound[i]..ubound[i]
     );
