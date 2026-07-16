include "fzn_bin_packing.mzn";
include "fzn_bin_packing_reif.mzn";
include "fzn_bin_packing_opt.mzn";

predicate fzn_bin_packing(int: capacity, array [int] of var int: bin, array [int] of int: weight);
predicate fzn_bin_packing_opt(int: capacity, array [int] of var opt int: bin, array [int] of int: weight);

/**  @group globals.packing
  Requires that each item \p i with weight \a weight[\p i], be put into \a bin[\p i] such
  that the sum of the weight of the items in each bin does not exceed the
  capacity \a capacity.

  Assumptions:

  - forall \p i, \a weight[\p i] >=0
  - \a capacity >=0

*/
predicate bin_packing(int: capacity, array [$$I] of var $$B: bin, array [$$I] of int: weight) =
  assert(
    index_set(bin) == index_set(weight),
    "bin_packing: the bin and weight arrays must have identical index sets",
    assert(
      length(weight) == 0 \/ lb_array(weight) >= 0,
      "bin_packing: the weight must be non-negative",
      assert(
        capacity >= 0,
        "bin_packing: capacity must be non-negative",
        fzn_bin_packing(capacity, index2int(enum2int(bin)), index2int(weight)),
      ),
    ),
  );

/**  @group globals.packing
  Requires that each item \p i with weight \a weight[\p i], be put into \a bin[\p i] such
  that the sum of the weight of the items in each bin does not exceed the
  capacity \a capacity. An item may not be put into any bin, in which case \a bin[\p i] is
  absent.

  Assumptions:

  - forall \p i, \a weight[\p i] >=0
  - \a capacity >=0

*/
predicate bin_packing(int: capacity, array [$$I] of var opt $$B: bin, array [$$I] of int: weight) =
  assert(
    index_set(bin) == index_set(weight),
    "bin_packing: the bin and weight arrays must have identical index sets",
    assert(
      length(weight) == 0 \/ lb_array(weight) >= 0,
      "bin_packing: the weight must be non-negative",
      assert(
        capacity >= 0,
        "bin_packing: capacity must be non-negative",
        fzn_bin_packing_opt(capacity, index2int(enum2int(bin)), index2int(weight)),
      ),
    ),
  );

%-----------------------------------------------------------------------------%
