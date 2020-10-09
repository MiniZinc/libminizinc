predicate fzn_at_most1(array[int] of var set of int: s) =
    forall(i,j in index_set(s) where i < j) (
         card(s[i] intersect s[j]) <= 1 );

