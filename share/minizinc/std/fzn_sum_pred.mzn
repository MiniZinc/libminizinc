predicate fzn_sum_pred(var int: i, array[int] of set of int: sets,
                   array[int] of int: cs, var int: s) =
    let {
        array[index_set(sets)] of int: sums = [ sum(k in sets[j])(cs[k]) | j in index_set(sets) ];
    } in sums[i] = s; 

