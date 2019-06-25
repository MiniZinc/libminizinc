predicate fzn_inverse(array[int] of var int: f,
                  array[int] of var int: invf) =
    forall(i in index_set(f), j in index_set(invf)) (
        f[i] in index_set(invf) /\
        invf[j] in index_set(f) /\
        (j == f[i] <-> i == invf[j])
    );
