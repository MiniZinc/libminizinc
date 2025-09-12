include "fzn_bin_packing_capa.mzn";
include "fzn_bin_packing_capa_reif.mzn";

/**  @group globals.packing
  Requires that each item \p i with weight \a w[\p i], be put into \a bin[\p i] such
  that the sum of the weights of the items in each bin \p b does not exceed the
  capacity \a c[\p b].

  Assumptions:

  - forall \p i, \a w[\p i] >=0
  - forall \p b, \a c[\p b] >=0
*/
predicate bin_packing_capa(
  array [$$B] of int: c,
  array [$$I] of var $$B: bin,
  array [$$I] of int: w,
) =
  assert(
    index_set(bin) = index_set(w),
    "bin_packing_capa: the bin and weight arrays must have identical index sets",
    assert(
      length(w) == 0 \/ lb_array(w) >= 0,
      "bin_packing_capa: the weights must be non-negative",
      assert(
        length(c) == 0 \/ lb_array(c) >= 0,
        "bin_packing_capa: the capacities must be non-negative",
        fzn_bin_packing_capa(index2int(c), index2int(enum2int(bin)), index2int(w)),
      ),
    ),
  );

%-----------------------------------------------------------------------------%
