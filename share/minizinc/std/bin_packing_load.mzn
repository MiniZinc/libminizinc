include "bin_packing_load_fn.mzn";

include "fzn_bin_packing_load.mzn";
include "fzn_bin_packing_load_opt.mzn";
include "fzn_bin_packing_load_reif.mzn";

predicate fzn_bin_packing_load(
  array [int] of var int: loads,
  array [int] of var int: bins,
  array [int] of int: weights,
);
predicate fzn_bin_packing_load_opt(
  array [int] of var int: loads,
  array [int] of var opt int: bins,
  array [int] of int: weights,
);

/**  @group globals.packing
  Requires that each item \p i with weight \a weights[\p i], be put into \a bins[\p i] such
  that the sum of the weights of the items in each bins \p b is equal to
  \a loads[\p b].

  Assumptions:

  - forall \p i, \a weights[\p i] >=0

*/
predicate bin_packing_load(
  array [$$B] of var int: loads,
  array [$$I] of var $$B: bins,
  array [$$I] of int: weights,
) =
  assert(
    index_set(bins) == index_set(weights),
    "bin_packing_load: the bins and weight arrays must have identical index sets",
    assert(
      length(weights) == 0 \/ lb_array(weights) >= 0,
      "bin_packing_load: the weights must be non-negative",
      fzn_bin_packing_load(index2int(loads), index2int(enum2int(bins)), index2int(weights)),
    ),
  );

/**  @group globals.packing
  Requires that each item \p i with weight \a weights[\p i], be put into \a bins[\p i] such
  that the sum of the weights of the items in each bins \p b is equal to
  \a loads[\p b]. An item may not be put into any bins, in which case \a bins[\p i] is
  absent.

  Assumptions:

  - forall \p i, \a weights[\p i] >=0

*/
predicate bin_packing_load(
  array [$$B] of var int: loads,
  array [$$I] of var opt $$B: bins,
  array [$$I] of int: weights,
) =
  assert(
    index_set(bins) == index_set(weights),
    "bin_packing_load: the bins and weight arrays must have identical index sets",
    assert(
      length(weights) == 0 \/ lb_array(weights) >= 0,
      "bin_packing_load: the weights must be non-negative",
      fzn_bin_packing_load_opt(index2int(loads), index2int(enum2int(bins)), index2int(weights)),
    ),
  );
