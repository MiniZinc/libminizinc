predicate fzn_seq_precede_chain_set(array[int] of var set of int: X) =
    let {
        set of int: S = ub_array(X); % set of possible values
        int : l = min (S) ;     % least possible value
        int : u = max (S) ;     % greatest possible value
        int : f = min ( index_set (X ));
        array [ index_set (X) ] of var l .. u: H;
    } in
        H[f] <= 1 /\ H[f] = max (X[f] union {0}) /\
        forall ( i in index_set (X) diff {f} ) (
            H[i] <= H[i-1] + 1 /\ H[i] = max (max(X[i]), H[i-1])
        );
