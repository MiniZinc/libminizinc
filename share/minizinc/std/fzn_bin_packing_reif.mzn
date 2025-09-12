predicate fzn_bin_packing_reif(
  int: c,
  array [int] of var int: bin,
  array [int] of int: w,
  var bool: b,
) =
  b <->
    if length(bin) > 0 then
      forall (assigned_bin in lb_array(bin)..ub_array(bin)) (
        sum (i in index_set(bin)) (w[i] * (bin[i] == assigned_bin)) <= c
      )
    endif;

%-----------------------------------------------------------------------------%
