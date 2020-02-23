include "fzn_global_cardinality_low_up.mzn";
include "fzn_global_cardinality_low_up_reif.mzn";
   
/** @group globals.counting
  Requires that for all \p i, the value \a cover[\p i] appears at least \a lbound[\p i]
  and at most \a ubound[\p i] times in the array \a x.
*/
predicate global_cardinality_low_up(array[int] of var int: x,
                                    array[int] of int: cover,
                                    array[int] of int: lbound,
                                    array[int] of int: ubound) =
  assert(
    index_set(cover) = index_set(lbound) /\ index_set(cover) = index_set(ubound),
    "global_cardinality_low_up: " ++
      "cover, lbound and ubound must have identical index sets",
    fzn_global_cardinality_low_up(x, cover, lbound, ubound)
  );
