predicate fzn_bin_packing_load(
  array [int] of var int: loads,
  array [int] of var int: bins,
  array [int] of int: weights,
) =
  sum(loads) = sum(weights) /\
    forall (i in index_set(bins)) (
      min(index_set(loads)) <= bins[i] /\ bins[i] <= max(index_set(loads))
    ) /\
    forall (b in index_set(loads)) (loads[b] = sum (i in index_set(bins)) (weights[i] * (bins[i] = b)));

%-----------------------------------------------------------------------------%
