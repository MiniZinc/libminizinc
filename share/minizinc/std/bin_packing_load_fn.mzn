include "bin_packing_load.mzn";

/**  @group globals.packing
  Returns the load of each bins resulting from packing each item \p i with
  weight \a weights[\p i] into \a bins[\p i], where the load is defined as
  the sum of the weights of the items in each bins.

  Assumptions:

  - forall \p i, \a weights[\p i] >=0
*/
function array [$$B] of var int: bin_packing_load(
  array [$$I] of var $$B: bins,
  array [$$I] of int: weights,
) :: promise_total =
  assert(
    index_set(bins) == index_set(weights),
    "bin_packing_load: the bins and weight arrays must have identical index sets",
    if length(bins) > 0 then
      let {
        array [dom_bounds_array(bins)] of var 0..sum(weights): load :: is_defined_var;
        constraint
          bin_packing_load(index2int(load), index2int(enum2int(bins)), index2int(weights))
            :: defines_var(load);
      } in load
    endif,
  );

/**  @group globals.packing
  Returns the load of each bins resulting from packing each item \p i with
  weight \a weights[\p i] into \a bins[\p i], where the load is defined as
  the sum of the weights of the items in each bins. An item may not be put into any bins,
  in which case \a bins[\p i] is absent.

  Assumptions:

  - forall \p i, \a weights[\p i] >=0
*/
function array [$$B] of var int: bin_packing_load(
  array [$$I] of var opt $$B: bins,
  array [$$I] of int: weights,
) :: promise_total =
  assert(
    index_set(bins) == index_set(weights),
    "bin_packing_load: the bins and weight arrays must have identical index sets",
    if length(bins) > 0 then
      let {
        array [dom_bounds_array(bins)] of var 0..sum(weights): load :: is_defined_var;
        constraint
          bin_packing_load(index2int(load), index2int(enum2int(bins)), index2int(weights))
            :: defines_var(load);
      } in load
    endif,
  );
