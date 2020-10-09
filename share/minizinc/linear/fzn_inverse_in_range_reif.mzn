predicate fzn_inverse_in_range_reif(array[int] of var int: f,
                           array[int] of var int: invf,
                           var bool: b) =
    b <-> forall(i in index_set(f), j in index_set(invf)) (
        (j == f[i] <-> i == invf[j])
    );
