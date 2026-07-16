predicate fzn_bin_packing(int: capacity, array [int] of var int: bin, array [int] of int: weight) =
  if length(bin) > 0 then
    let {
      int: all_weight = sum(weight);
    } in forall (b in lb_array(bin)..ub_array(bin)) (
      sum (i in index_set(bin)) (weight[i] * (bin[i] != b)) >= (all_weight - capacity)
    )
  endif;

%-----------------------------------------------------------------------------%
