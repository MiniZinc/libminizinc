predicate fzn_bin_packing_reif(int: c,
                      array[int] of var int: bin,
                      array[int] of int: w,
                      var bool: b) =
        b <-> forall( b in lb_array(bin)..ub_array(bin) ) (
            c >= sum ( i in index_set(bin) ) (
                w[i] * bool2int( bin[i] == b )
            )
        );

%-----------------------------------------------------------------------------%
