include "bin_packing_capa.mzn";

predicate fzn_bin_packing_opt(int: c, array [int] of var opt int: bin, array [int] of int: w) =
  if length(bin) > 0 then
  let {
    any: dummy_bin = ub_array(bin) + 1;
    any: new_bins = lb_array(bin) .. dummy_bin;
    any: dummy_capa = sum(w);
  } in
    bin_packing_capa([b: if b < dummy_bin then c else dummy_capa endif | b in new_bins], [b default dummy_bin | b in bin], w)
  endif;

%-----------------------------------------------------------------------------%
