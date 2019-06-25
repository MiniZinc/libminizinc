predicate fzn_bin_packing_load_reif(array[int] of var int: load,
                           array[int] of var int: bin,
                           array[int] of int: w,
                           var bool: b) =
    b <-> (
        sum(load) = sum(w)
    /\  forall( i in index_set(bin) ) (
            min(index_set(load)) <= bin[i] /\ bin[i] <= max(index_set(load))
        )
    /\  forall( b in index_set(load) ) (
            load[b] = sum ( i in index_set(bin) ) (
                w[i] * bool2int( bin[i] = b )
            )
        ) );

%-----------------------------------------------------------------------------%
