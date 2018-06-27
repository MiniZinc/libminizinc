include "bin_packing_load.mzn";

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
         constraint bin_packing_load(load,bin,w); }
   in load;
