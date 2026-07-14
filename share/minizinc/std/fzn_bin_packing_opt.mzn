include "bin_packing_capa.mzn";

predicate fzn_bin_packing_opt(
  int: capacity,
  array [int] of var opt int: bins,
  array [int] of int: weights,
) =
  if length(bins) > 0 then
  let {
    any: dummy_bin = ub_array(bins) + 1;
    any: new_bins = lb_array(bins) .. dummy_bin;
    any: dummy_capa = sum(weights);
  } in
    bin_packing_capa(
      [b: if b < dummy_bin then capacity else dummy_capa endif | b in new_bins],
      [b default dummy_bin | b in bins],
      weights,
    )
  endif;

%-----------------------------------------------------------------------------%
