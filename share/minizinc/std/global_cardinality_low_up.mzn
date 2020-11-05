include "fzn_global_cardinality_low_up.mzn";
include "fzn_global_cardinality_low_up_reif.mzn";
   
/** @group globals.counting
  Requires that for all \p i, the value \a cover[\p i] appears at least \a lbound[\p i]
  and at most \a ubound[\p i] times in the array \a x.
*/
predicate global_cardinality_low_up(array[$X] of var int: x,
                                    array[$Y] of int: cover,
                                    array[$Y] of int: lbound,
                                    array[$Y] of int: ubound) =
  assert(
    index_sets_agree(cover,lbound) /\ index_sets_agree(cover,ubound),
    "global_cardinality_low_up: " ++
      "cover, lbound and ubound must have identical index sets",
    fzn_global_cardinality_low_up(array1d(x), array1d(cover), array1d(lbound), array1d(ubound))
  );
