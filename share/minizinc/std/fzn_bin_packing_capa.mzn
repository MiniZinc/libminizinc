predicate fzn_bin_packing_capa(
  array [int] of int: capacities,
  array [int] of var int: bins,
  array [int] of int: weights,
) =
  forall (i in index_set(bins)) (
    min(index_set(capacities)) <= bins[i] /\ bins[i] <= max(index_set(capacities))
  ) /\
    forall (b in index_set(capacities)) (
      capacities[b] >= sum (i in index_set(bins)) (weights[i] * (bins[i] = b))
    );

%-----------------------------------------------------------------------------%
