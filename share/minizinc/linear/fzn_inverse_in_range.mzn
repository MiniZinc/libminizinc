predicate fzn_inverse_in_range(array[int] of var int: f,
                  array[int] of var int: invf) =
    forall(i in index_set(f), j in index_set(invf)) (
        (j == f[i] <-> i == invf[j])
    );
