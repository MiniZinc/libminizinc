predicate fzn_bin_packing_capa_reif(array[int] of int: c,
                           array[int] of var int: bin,
                           array[int] of int: w,
                           var bool: b) =
   b <-> (
      forall( i in index_set(bin) ) (
            min(index_set(c)) <= bin[i] /\ bin[i] <= max(index_set(c))
      )
    /\
      forall( b in index_set(c) ) (
            c[b] >= sum ( i in index_set(bin) ) (
                w[i] * bool2int( bin[i] = b )
            )
      ));

%-----------------------------------------------------------------------------%
