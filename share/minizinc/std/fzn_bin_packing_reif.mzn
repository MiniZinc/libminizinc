predicate fzn_bin_packing_reif(
  int: capacity,
  array [int] of var int: bins,
  array [int] of int: weights,
  var bool: b,
) =
  b <->
    if length(bins) > 0 then
      forall (assigned_bin in lb_array(bins)..ub_array(bins)) (
        sum (i in index_set(bins)) (weights[i] * (bins[i] == assigned_bin)) <= capacity
      )
    endif;

%-----------------------------------------------------------------------------%
