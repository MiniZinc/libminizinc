predicate fzn_bin_packing_capa_reif(array[int] of int: c,
                           array[int] of var int: bin,
                           array[int] of int: w,
                           var bool: b) =
   b <-> (
      forall( i in index_set(bin) ) (
            min(index_set(c)) <= bin[i] /\ bin[i] <= max(index_set(c))
      )
    /\
      forall( assigned_bin in index_set(c) ) (
            c[assigned_bin] >= sum ( i in index_set(bin) ) (
                w[i] * ( bin[i] = assigned_bin )
            )
      ));

%-----------------------------------------------------------------------------%
