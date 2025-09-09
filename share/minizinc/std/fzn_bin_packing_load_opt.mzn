include "bin_packing_load.mzn";

predicate fzn_bin_packing_load_opt(
  array [int] of var int: load,
  array [int] of var opt int: bin,
  array [int] of int: w,
 ) =
  let {
    any: dummy_bin = max(index_set(load)) + 1;
    any: new_bins = min(index_set(load)) .. dummy_bin;
    var 0..sum(w): dummy_load;
  } in
    bin_packing_load([b: if b < dummy_bin then load[b] else dummy_load endif | b in new_bins], [b default dummy_bin | b in bin], w)
  ;

%-----------------------------------------------------------------------------%
