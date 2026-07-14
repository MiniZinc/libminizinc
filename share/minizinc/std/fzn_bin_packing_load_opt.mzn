include "bin_packing_load.mzn";

predicate fzn_bin_packing_load_opt(
  array [int] of var int: loads,
  array [int] of var opt int: bins,
  array [int] of int: weights,
 ) =
  let {
    any: dummy_bin = max(index_set(loads)) + 1;
    any: new_bins = min(index_set(loads)) .. dummy_bin;
    var 0..sum(weights): dummy_load;
  } in
    bin_packing_load([b: if b < dummy_bin then loads[b] else dummy_load endif | b in new_bins], [b default dummy_bin | b in bins], weights)
  ;

%-----------------------------------------------------------------------------%
