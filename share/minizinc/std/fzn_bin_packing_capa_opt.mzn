include "bin_packing_capa.mzn";

predicate fzn_bin_packing_capa_opt(
  array [int] of int: capacity,
  array [int] of var opt int: bin,
  array [int] of int: weight,
) =
  if length(bin) > 0 then
  let {
    any: dummy_bin = max(index_set(capacity)) + 1;
    any: new_bins = min(index_set(capacity)) .. dummy_bin;
    any: dummy_capa = sum(weight);
  } in
    bin_packing_capa([b: if b < dummy_bin then capacity[b] else dummy_capa endif | b in new_bins], [b default dummy_bin | b in bin], weight)
  endif;

%-----------------------------------------------------------------------------%
