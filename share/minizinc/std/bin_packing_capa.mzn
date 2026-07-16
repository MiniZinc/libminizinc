include "fzn_bin_packing_capa.mzn";
include "fzn_bin_packing_capa_reif.mzn";
include "fzn_bin_packing_capa_opt.mzn";

predicate fzn_bin_packing_capa(
  array [int] of int: capacity,
  array [int] of var int: bin,
  array [int] of int: weight,
);
predicate fzn_bin_packing_capa_opt(
  array [int] of int: capacity,
  array [int] of var opt int: bin,
  array [int] of int: weight,
);

/**  @group globals.packing
  Requires that each item \p i with weight \a weight[\p i], be put into \a bin[\p i] such
  that the sum of the weight of the items in each bin \p b does not exceed the
  capacity \a capacity[\p b].

  Assumptions:

  - forall \p i, \a weight[\p i] >=0
  - forall \p b, \a capacity[\p b] >=0
*/
predicate bin_packing_capa(
  array [$$B] of int: capacity,
  array [$$I] of var $$B: bin,
  array [$$I] of int: weight,
) =
  assert(
    index_set(bin) = index_set(weight),
    "bin_packing_capa: the bin and weight arrays must have identical index sets",
    assert(
      length(weight) == 0 \/ lb_array(weight) >= 0,
      "bin_packing_capa: the weight must be non-negative",
      assert(
        length(capacity) == 0 \/ lb_array(capacity) >= 0,
        "bin_packing_capa: the capacity must be non-negative",
        fzn_bin_packing_capa(index2int(capacity), index2int(enum2int(bin)), index2int(weight)),
      ),
    ),
  );

/**  @group globals.packing
  Requires that each item \p i with weight \a weight[\p i], be put into \a bin[\p i] such
  that the sum of the weight of the items in each bin \p b does not exceed the
  capacity \a capacity[\p b]. An item may not be put into any bin, in which case \a bin[\p i] is
  absent.

  Assumptions:

  - forall \p i, \a weight[\p i] >=0
  - forall \p b, \a capacity[\p b] >=0
*/
predicate bin_packing_capa(
  array [$$B] of int: capacity,
  array [$$I] of var opt $$B: bin,
  array [$$I] of int: weight,
) =
  assert(
    index_set(bin) = index_set(weight),
    "bin_packing_capa: the bin and weight arrays must have identical index sets",
    assert(
      length(weight) == 0 \/ lb_array(weight) >= 0,
      "bin_packing_capa: the weight must be non-negative",
      assert(
        length(capacity) == 0 \/ lb_array(capacity) >= 0,
        "bin_packing_capa: the capacity must be non-negative",
        fzn_bin_packing_capa_opt(index2int(capacity), index2int(enum2int(bin)), index2int(weight)),
      ),
    ),
  );

%-----------------------------------------------------------------------------%
