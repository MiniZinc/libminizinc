predicate fzn_bin_packing(int: capacity, array [int] of var int: bins, array [int] of int: weights) =
  if length(bins) > 0 then
    let {
      int: all_weight = sum(weights);
    } in forall (b in lb_array(bins)..ub_array(bins)) (
      sum (i in index_set(bins)) (weights[i] * (bins[i] != b)) >= (all_weight - capacity)
    )
  endif;

%-----------------------------------------------------------------------------%
