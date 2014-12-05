predicate bool_clause_reif(array[int] of var bool: p, array[int] of var bool: n, var bool: c) =
    c = ( sum(i in index_set(p))( bool2int(p[i]) )
        - sum(i in index_set(n))( bool2int(n[i]) )
        + length(n)
        >= 1
    );
