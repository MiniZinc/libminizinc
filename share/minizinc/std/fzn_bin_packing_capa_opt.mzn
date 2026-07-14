include "bin_packing_capa.mzn";

predicate fzn_bin_packing_capa_opt(
  array [int] of int: capacities,
  array [int] of var opt int: bins,
  array [int] of int: weights,
) =
  if length(bins) > 0 then
  let {
    any: dummy_bin = max(index_set(capacities)) + 1;
    any: new_bins = min(index_set(capacities)) .. dummy_bin;
    any: dummy_capa = sum(weights);
  } in
    bin_packing_capa([b: if b < dummy_bin then capacities[b] else dummy_capa endif | b in new_bins], [b default dummy_bin | b in bins], weights)
  endif;

%-----------------------------------------------------------------------------%
