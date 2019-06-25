predicate fzn_distribute_reif(array[int] of var int: card,
                              array[int] of var int: value,
                              array[int] of var int: base,
                              var bool: b) =
        b <-> forall (i in index_set(card)) (
            card[i] == sum(j in index_set(base)) (
                            bool2int(value[i] = base[j])
                       )
        );
