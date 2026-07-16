predicate fzn_bin_packing_reif(
  int: capacity,
  array [int] of var int: bin,
  array [int] of int: weight,
  var bool: b,
) =
  b <->
    if length(bin) > 0 then
      forall (assigned_bin in lb_array(bin)..ub_array(bin)) (
        sum (i in index_set(bin)) (weight[i] * (bin[i] == assigned_bin)) <= capacity
      )
    endif;

%-----------------------------------------------------------------------------%
