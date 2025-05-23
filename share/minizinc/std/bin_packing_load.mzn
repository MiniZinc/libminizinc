include "bin_packing_load_fn.mzn";

include "fzn_bin_packing_load.mzn";
include "fzn_bin_packing_load_reif.mzn";

/**  @group globals.packing
  Requires that each item \p i with weight \a w[\p i], be put into \a bin[\p i] such
  that the sum of the weights of the items in each bin \p b is equal to
  \a load[\p b].

  Assumptions:

  - forall \p i, \a w[\p i] >=0

*/
predicate bin_packing_load(
  array [$$B] of var int: load,
  array [$$I] of var $$B: bin,
  array [$$I] of int: w,
) =
  assert(
    index_set(bin) == index_set(w),
    "bin_packing_load: the bin and weight arrays must have identical index sets",
    assert(
      length(w) == 0 \/ lb_array(w) >= 0,
      "bin_packing_load: the weights must be non-negative",
      fzn_bin_packing_load(index2int(load), index2int(enum2int(bin)), index2int(w)),
    ),
  );
