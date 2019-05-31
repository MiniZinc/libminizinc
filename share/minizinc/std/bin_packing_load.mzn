/**  @group globals.packing
  Requires that each item \p i with weight \a w[\p i], be put into \a bin[\p i] such 
  that the sum of the weights of the items in each bin \p b is equal to
  \a load[\p b].

  Assumptions:
  - forall \p i, \a w[\p i] >=0
*/
predicate bin_packing_load(array[int] of var int: load,
                           array[int] of var int: bin,
                           array[int] of int: w) =
    assert(index_set(bin) == index_set(w),
        "bin_packing_load: the bin and weight arrays must have identical index sets",
    assert(lb_array(w) >= 0,
        "bin_packing_load: the weights must be non-negative",
      fzn_bin_packing_load(load, bin, w)
    ));

/**  @group globals.packing
  Returns the load of each bin resulting from packing each item \p i with
  weight \a w[\p i] into \a bin[\p i], where the load is defined as 
  the sum of the weights of the items in each bin.

  Assumptions:
  - forall \p i, \a w[\p i] >=0
*/
function array[int] of var int: bin_packing_load(array[int] of var int: bin,
                                                 array[int] of int: w) :: promise_total =
   let { array[dom_bounds_array(bin)] of var 0..sum(w): load;
         constraint fzn_bin_packing_load(load,bin,w); }
   in load;


include "fzn_bin_packing_load.mzn";
%-----------------------------------------------------------------------------%
