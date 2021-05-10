predicate fzn_bin_packing(int: c,
                      array[int] of var int: bin,
                      array[int] of int: w) =
    let {
        int: all_weight = sum(w)
    } in forall( b in lb_array(bin)..ub_array(bin) ) (
        sum(i in index_set(bin)) (
            w[i] * (bin[i] != b)
        ) >= (all_weight - c)
    );

%-----------------------------------------------------------------------------%
