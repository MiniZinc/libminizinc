predicate fzn_global_cardinality_low_up_set(array[int] of var set of int: x,
                                            array[int] of int: cover,
                                            array[int] of int: lbound,
                                            array[int] of int: ubound) =
    forall (i in index_set(cover)) (
        let {
            var int: sd = count (v in x) (cover[i] in v);
        } in lbound[i] <= sd /\ sd <= ubound[i]
    );
