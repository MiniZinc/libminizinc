include "fzn_global_cardinality.mzn";
include "fzn_global_cardinality_reif.mzn";
include "fzn_global_cardinality_opt.mzn";
include "fzn_global_cardinality_set.mzn";

include "fzn_global_cardinality_low_up.mzn";
include "fzn_global_cardinality_low_up_reif.mzn";
include "fzn_global_cardinality_low_up_opt.mzn";
include "fzn_global_cardinality_low_up_set.mzn";

/** @group globals.counting
  Requires that the number of occurrences of \a cover[\p i] in \a x is \a counts[\p i].
*/
predicate global_cardinality(array[$X] of var int: x,
                             array[$Y] of int: cover,
                             array[$Y] of var int: counts) =
    assert(index_sets_agree(cover, counts),
        "global_cardinality: cover and counts must have identical index sets",
        if length(x) == 0 then
          forall(c in array1d(counts))(c = 0)
        else
          fzn_global_cardinality(array1d(x), array1d(cover), array1d(counts))
        endif
    );

/** @group globals.counting
  Requires that the number of occurrences of \a cover[\p i] in \a x is \a counts[\p i].
*/
predicate global_cardinality(array[$X] of var opt int: x,
                             array[$Y] of int: cover,
                             array[$Y] of var int: counts) =
    assert(index_sets_agree(cover, counts),
        "global_cardinality: cover and counts must have identical index sets",
        if length(x) == 0 then
          forall(c in array1d(counts))(c = 0)
        else
          fzn_global_cardinality_opt(array1d(x), array1d(cover), array1d(counts))
        endif
    );

/** @group globals.counting
  Requires that for all \p i, the value \a cover[\p i] appears at least \a lbound[\p i]
  and at most \a ubound[\p i] times in the array \a x.
*/
predicate global_cardinality(array[$X] of var int: x,
                             array[$Y] of int: cover,
                             array[$Y] of int: lbound,
                             array[$Y] of int: ubound) =
  assert(
    index_sets_agree(cover,lbound) /\ index_sets_agree(cover,ubound),
    "global_cardinality_low_up: " ++
      "cover, lbound and ubound must have identical index sets",
    if length(x) == 0 then
          assert(forall(l in array1d(lbound))( l <= 0) /\ forall(u in array1d(ubound))( u >= 0)
            \/ length(cover) == 0,
            "global_cardinality_low_up: " ++
            "lbound and ubound must allow a count of 0 when x is empty, or also be empty",
            true)
    else
      fzn_global_cardinality_low_up(array1d(x), array1d(cover), array1d(lbound), array1d(ubound))
    endif
  );

/** @group globals.counting
  Requires that for all \p i, the value \a cover[\p i] appears at least \a lbound[\p i]
  and at most \a ubound[\p i] times in the array \a x.
*/
predicate global_cardinality(array[$X] of var opt int: x,
                             array[$Y] of int: cover,
                             array[$Y] of int: lbound,
                             array[$Y] of int: ubound) =
  assert(
    index_sets_agree(cover,lbound) /\ index_sets_agree(cover,ubound),
    "global_cardinality_low_up: " ++
      "cover, lbound and ubound must have identical index sets",
    if length(x) == 0 then
          assert(forall(l in array1d(lbound))( l <= 0) /\ forall(u in array1d(ubound))( u >= 0)
            \/ length(cover) == 0,
            "global_cardinality_low_up: " ++
            "lbound and ubound must allow a count of 0 when x is empty, or also be empty",
            true)
    else
      fzn_global_cardinality_low_up_opt(array1d(x), array1d(cover), array1d(lbound), array1d(ubound))
    endif
  );

/** @group globals.counting
  Requires that the number of occurrences of \a cover[\p i] in \a x is \a counts[\p i].
*/
predicate global_cardinality(array[$X] of var set of int: x,
                             array[$Y] of int: cover,
                             array[$Y] of var int: counts) = 
    assert(index_sets_agree(cover, counts),
        "global_cardinality: cover and counts must have identical index sets",
        if length(x) == 0 then
          forall(c in array1d(counts))(c = 0)
        else
          fzn_global_cardinality_set(array1d(x), array1d(cover), array1d(counts))
        endif
    );

/** @group globals.counting
  Requires that for all \p i, the value \a cover[\p i] appears at least \a lbound[\p i]
  and at most \a ubound[\p i] times in the array \a x.
*/
predicate global_cardinality(array[$X] of var set of int: x,
                             array[$Y] of int: cover,
                             array[$Y] of int: lbound,
                             array[$Y] of int: ubound) = 
  assert(
    index_sets_agree(cover,lbound) /\ index_sets_agree(cover,ubound),
    "global_cardinality_low_up: " ++
      "cover, lbound and ubound must have identical index sets",
    if length(x) == 0 then
          assert(forall (l in lbound) (l <= 0)
              /\ forall (u in ubound) (u >= 0)
              \/ length(cover) == 0,
            "global_cardinality_low_up: " ++
            "lbound and ubound must allow a count of 0 when x is empty, or also be empty",
            true)
    else
      fzn_global_cardinality_low_up_set(array1d(x), array1d(cover), array1d(lbound), array1d(ubound))
    endif
  );
