include "fzn_bin_packing_capa.mzn";
include "fzn_bin_packing_capa_reif.mzn";
include "fzn_bin_packing_capa_opt.mzn";

/**  @group globals.packing
  Requires that each item \p i with weight \a weights[\p i], be put into \a bins[\p i] such
  that the sum of the weights of the items in each bins \p b does not exceed the
  capacity \a capacities[\p b].

  Assumptions:

  - forall \p i, \a weights[\p i] >=0
  - forall \p b, \a capacities[\p b] >=0
*/
predicate bin_packing_capa(
  array [$$B] of int: capacities,
  array [$$I] of var $$B: bins,
  array [$$I] of int: weights,
) =
  assert(
    index_set(bins) = index_set(weights),
    "bin_packing_capa: the bins and weight arrays must have identical index sets",
    assert(
      length(weights) == 0 \/ lb_array(weights) >= 0,
      "bin_packing_capa: the weights must be non-negative",
      assert(
        length(capacities) == 0 \/ lb_array(capacities) >= 0,
        "bin_packing_capa: the capacities must be non-negative",
        fzn_bin_packing_capa(index2int(capacities), index2int(enum2int(bins)), index2int(weights)),
      ),
    ),
  );

/**  @group globals.packing
  Requires that each item \p i with weight \a weights[\p i], be put into \a bins[\p i] such
  that the sum of the weights of the items in each bins \p b does not exceed the
  capacity \a capacities[\p b]. An item may not be put into any bins, in which case \a bins[\p i] is
  absent.

  Assumptions:

  - forall \p i, \a weights[\p i] >=0
  - forall \p b, \a capacities[\p b] >=0
*/
predicate bin_packing_capa(
  array [$$B] of int: capacities,
  array [$$I] of var opt $$B: bins,
  array [$$I] of int: weights,
) =
  assert(
    index_set(bins) = index_set(weights),
    "bin_packing_capa: the bins and weight arrays must have identical index sets",
    assert(
      length(weights) == 0 \/ lb_array(weights) >= 0,
      "bin_packing_capa: the weights must be non-negative",
      assert(
        length(capacities) == 0 \/ lb_array(capacities) >= 0,
        "bin_packing_capa: the capacities must be non-negative",
        fzn_bin_packing_capa_opt(index2int(capacities), index2int(enum2int(bins)), index2int(weights)),
      ),
    ),
  );

%-----------------------------------------------------------------------------%
