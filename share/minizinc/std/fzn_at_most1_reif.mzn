predicate fzn_at_most1_reif(array[int] of var set of int: s, var bool: b) =
    b <-> forall(i,j in index_set(s) where i < j) (
            card(s[i] intersect s[j]) <= 1 );

