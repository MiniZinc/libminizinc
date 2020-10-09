predicate fzn_link_set_to_booleans(var set of int: s, array[int] of var bool: b) =
    forall(i in index_set(b)) ( b[i] <-> i in s );
