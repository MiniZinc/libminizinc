predicate fzn_value_precede_set(int: s, int: t, array[int] of var set of int: x) =
    let {
        int: imin = min(index_set(x)),
        int: imax = max(index_set(x)),
        array[imin..imax + 1] of var bool: b
    } in (
        forall (i in imin..imax) (let {
            var bool: xis = (s in x[i] /\ not (t in x[i]))
        } in
            (xis -> (b[i + 1] == true))
        /\  ((not xis) -> (b[i] == b[i + 1]))
        /\  ((not b[i]) -> (s in x[i] \/ not (t in x[i])))
        )
    /\  b[imin] == false
    );



predicate fzn_value_precede_set_reif(int: s, int: t, 
                           array[int] of var set of int: x, var bool: b) =
   abort("Reified value_precede/3 for sets is not supported.");
