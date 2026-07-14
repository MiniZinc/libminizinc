predicate fzn_bin_packing_capa_reif(
  array [int] of int: capacities,
  array [int] of var int: bins,
  array [int] of int: weights,
  var bool: b,
) =
  b <->
    (
      forall (i in index_set(bins)) (min(index_set(capacities)) <= bins[i] /\ bins[i] <= max(index_set(capacities))) /\
        forall (assigned_bin in index_set(capacities)) (
          capacities[assigned_bin] >= sum (i in index_set(bins)) (weights[i] * (bins[i] = assigned_bin))
        )
    );

%-----------------------------------------------------------------------------%
