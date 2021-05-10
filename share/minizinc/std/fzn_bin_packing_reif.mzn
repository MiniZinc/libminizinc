predicate fzn_bin_packing_reif(int: c,
                      array[int] of var int: bin,
                      array[int] of int: w,
                      var bool: b) =
        b <-> forall( assigned_bin in lb_array(bin)..ub_array(bin) ) (
            c >= sum ( i in index_set(bin) ) (
                w[i] * ( bin[i] == assigned_bin )
            )
        );

%-----------------------------------------------------------------------------%
