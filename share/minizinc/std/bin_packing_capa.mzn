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
predicate bin_packing_capa(array[int] of int: c,
                           array[int] of var int: bin,
                           array[int] of int: w) =
    assert(index_set(bin) = index_set(w),
        "bin_packing_capa: the bin and weight arrays must have identical index sets",
    assert(lb_array(w) >= 0,
        "bin_packing_capa: the weights must be non-negative",
    assert(lb_array(c) >= 0,
        "bin_packing_capa: the capacities must be non-negative",
        fzn_bin_packing_capa(c, bin, w)    
    )));

predicate bin_packing_capa_reif(array[int] of int: c,
                           array[int] of var int: bin,
                           array[int] of int: w,
                           var bool: b) =
    assert(index_set(bin) = index_set(w),
        "bin_packing_capa: the bin and weight arrays must have identical index sets",
    assert(lb_array(w) >= 0,
        "bin_packing_capa: the weights must be non-negative",
    assert(lb_array(c) >= 0,
        "bin_packing_capa: the capacities must be non-negative",
        fzn_bin_packing_capa_reif(c, bin, w, b)
    )));

%-----------------------------------------------------------------------------%
