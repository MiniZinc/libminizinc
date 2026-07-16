predicate fzn_bin_packing_capa_reif(
  array [int] of int: capacity,
  array [int] of var int: bin,
  array [int] of int: weight,
  var bool: b,
) =
  b <->
    (
      forall (i in index_set(bin)) (min(index_set(capacity)) <= bin[i] /\ bin[i] <= max(index_set(capacity))) /\
        forall (assigned_bin in index_set(capacity)) (
          capacity[assigned_bin] >= sum (i in index_set(bin)) (weight[i] * (bin[i] = assigned_bin))
        )
    );

%-----------------------------------------------------------------------------%
