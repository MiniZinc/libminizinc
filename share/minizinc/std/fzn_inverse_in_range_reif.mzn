predicate fzn_inverse_in_range_reif(array[int] of var int: f,
                           array[int] of var int: invf,
                           var bool: b) =
    b <-> (
      forall(i in index_set(f)) (
        f[i] in index_set(invf) ->
        (invf[f[i]] == i)
      ) /\
      forall(j in index_set(invf)) (
        invf[j] in index_set(f) ->
        (f[invf[j]] == j)
      )
    );

