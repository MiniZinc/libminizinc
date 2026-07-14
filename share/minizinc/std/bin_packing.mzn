include "fzn_bin_packing.mzn";
include "fzn_bin_packing_reif.mzn";
include "fzn_bin_packing_opt.mzn";

predicate fzn_bin_packing(int: capacity, array [int] of var int: bins, array [int] of int: weights);
predicate fzn_bin_packing_opt(int: capacity, array [int] of var opt int: bins, array [int] of int: weights);

/**  @group globals.packing
  Requires that each item \p i with weight \a weights[\p i], be put into \a bins[\p i] such
  that the sum of the weights of the items in each bins does not exceed the
  capacity \a capacity.

  Assumptions:

  - forall \p i, \a weights[\p i] >=0
  - \a capacity >=0

*/
predicate bin_packing(int: capacity, array [$$I] of var $$B: bins, array [$$I] of int: weights) =
  assert(
    index_set(bins) == index_set(weights),
    "bin_packing: the bins and weight arrays must have identical index sets",
    assert(
      length(weights) == 0 \/ lb_array(weights) >= 0,
      "bin_packing: the weights must be non-negative",
      assert(
        capacity >= 0,
        "bin_packing: capacity must be non-negative",
        fzn_bin_packing(capacity, index2int(enum2int(bins)), index2int(weights)),
      ),
    ),
  );

/**  @group globals.packing
  Requires that each item \p i with weight \a weights[\p i], be put into \a bins[\p i] such
  that the sum of the weights of the items in each bins does not exceed the
  capacity \a capacity. An item may not be put into any bins, in which case \a bins[\p i] is
  absent.

  Assumptions:

  - forall \p i, \a weights[\p i] >=0
  - \a capacity >=0

*/
predicate bin_packing(int: capacity, array [$$I] of var opt $$B: bins, array [$$I] of int: weights) =
  assert(
    index_set(bins) == index_set(weights),
    "bin_packing: the bins and weight arrays must have identical index sets",
    assert(
      length(weights) == 0 \/ lb_array(weights) >= 0,
      "bin_packing: the weights must be non-negative",
      assert(
        capacity >= 0,
        "bin_packing: capacity must be non-negative",
        fzn_bin_packing_opt(capacity, index2int(enum2int(bins)), index2int(weights)),
      ),
    ),
  );

%-----------------------------------------------------------------------------%
