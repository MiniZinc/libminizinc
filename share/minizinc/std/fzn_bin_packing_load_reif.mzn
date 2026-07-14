predicate fzn_bin_packing_load_reif(
  array [int] of var int: loads,
  array [int] of var int: bins,
  array [int] of int: weights,
  var bool: b,
) =
  b <->
    (
      sum(loads) = sum(weights) /\
        forall (i in index_set(bins)) (
          min(index_set(loads)) <= bins[i] /\ bins[i] <= max(index_set(loads))
        ) /\
        forall (assigned_bin in index_set(loads)) (
          loads[assigned_bin] = sum (i in index_set(bins)) (weights[i] * (bins[i] = assigned_bin))
        )
    );

%-----------------------------------------------------------------------------%
