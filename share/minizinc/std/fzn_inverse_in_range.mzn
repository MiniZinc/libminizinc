include "global_cardinality.mzn";

predicate fzn_inverse_in_range(array[int] of var int: f,
                  array[int] of var int: invf) =
    forall(i in index_set(f)) (
        f[i] in index_set(invf) ->
        (invf[f[i]] == i)
    ) /\
    forall(j in index_set(invf)) (
        invf[j] in index_set(f) ->
        (f[invf[j]] == j)
    ) /\
    redundant_constraint( strengthen_injection_for_inverse_in_range(f, invf) )
    /\
    redundant_constraint( strengthen_injection_for_inverse_in_range(invf, f) );

predicate strengthen_injection_for_inverse_in_range(array[int] of var int: f,
                  array[int] of var int: invf) =
    let {
      set of int: sinvf = { i | i in index_set(invf) where
                            dom(invf[i]) subset index_set(f) },
    } in
      global_cardinality(f,
        [ i | i in sinvf ],
        [ 1 | i in sinvf ]
      );
