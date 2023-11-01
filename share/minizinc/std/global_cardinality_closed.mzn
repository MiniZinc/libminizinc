include "fzn_global_cardinality_closed.mzn";
include "fzn_global_cardinality_closed_reif.mzn";
include "fzn_global_cardinality_closed_opt.mzn";
include "fzn_global_cardinality_closed_set.mzn";

include "fzn_global_cardinality_low_up_closed.mzn";
include "fzn_global_cardinality_low_up_closed_reif.mzn";
include "fzn_global_cardinality_low_up_closed_opt.mzn";
include "fzn_global_cardinality_low_up_closed_set.mzn";

/** @group globals.counting
  Requires that the number of occurrences of \a cover[\p i] in \a x is \a counts[\p i].

  The elements of \a x must take their values from \a cover.
*/
predicate global_cardinality_closed(array[$X] of var $$E: x,
                                    array[$Y] of $$E: cover,
                                    array[$Y] of var int: counts) =
    assert(index_sets_agree(cover, counts),
        "global_cardinality_closed: " ++
        "cover has index sets " ++ show_index_sets(cover) ++ " and count has index sets " ++ show_index_sets(counts) ++ ", but they must have identical index sets",
        if length(x) == 0 then
          forall(c in array1d(counts))(c = 0)
        else
          fzn_global_cardinality_closed(array1d(x), array1d(cover), array1d(counts))
        endif
    );

/** @group globals.counting
  Requires that the number of occurrences of \a cover[\p i] in \a x is \a counts[\p i].

  The elements of \a x must take their values from \a cover or be absent.
*/
predicate global_cardinality_closed(array[$X] of var opt $$E: x,
                                    array[$Y] of $$E: cover,
                                    array[$Y] of var int: counts) =
    assert(index_sets_agree(cover, counts),
        "global_cardinality_closed: " ++
        "cover and counts must have identical index sets",
        if length(x) == 0 then
          forall(c in array1d(counts))(c = 0)
        else
          fzn_global_cardinality_closed_opt(array1d(x), array1d(cover), array1d(counts))
        endif
    );

/** @group globals.counting
  Requires that for all \p i, the value \a cover[\p i] appears at least \a lbound[\p i]
  and at most \a ubound[\p i] times in the array \a x.

  The elements of \a x must take their values from \a cover.
*/
predicate global_cardinality_closed(array[$X] of var $$E: x,
                                    array[$Y] of $$E: cover,
                                    array[$Y] of int: lbound,
                                    array[$Y] of int: ubound) =
  assert(
   index_sets_agree(cover,lbound) /\ index_sets_agree(cover,ubound),
   "global_cardinality_low_up_closed: " ++
   "cover has index sets " ++ show_index_sets(cover) ++ ", lbound has index sets " ++ show_index_sets(lbound) ++ ", and ubound has index sets " ++ show_index_sets(lbound) ++ ", but they must have identical index sets",
   if length(x) == 0 then
          assert(forall(l in array1d(lbound))( l <= 0) /\ forall(u in array1d(ubound))( u >= 0)
            \/ length(cover) == 0,
            "global_cardinality_low_up_closed: " ++
            "lbound and ubound must allow a count of 0 when x is empty, or also be empty",
            true)
   elseif length(cover) == 0 then
     assert(false,"global_cardinality_low_up_closed: " ++
            "cover must be empty when x is empty",
            false)
   else
     fzn_global_cardinality_low_up_closed(array1d(x), array1d(cover), array1d(lbound), array1d(ubound))
   endif
  );

/** @group globals.counting
  Requires that for all \p i, the value \a cover[\p i] appears at least \a lbound[\p i]
  and at most \a ubound[\p i] times in the array \a x.

  The elements of \a x must take their values from \a cover or be absent.
*/
predicate global_cardinality_closed(array[$X] of var opt $$E: x,
                                    array[$Y] of $$E: cover,
                                    array[$Y] of int: lbound,
                                    array[$Y] of int: ubound) =
  assert(
   index_sets_agree(cover,lbound) /\ index_sets_agree(cover,ubound),
   "global_cardinality_low_up_closed: " ++
   "cover has index sets " ++ show_index_sets(cover) ++ ", lbound has index sets " ++ show_index_sets(lbound) ++ ", and ubound has index sets " ++ show_index_sets(lbound) ++ ", but they must have identical index sets",
   if length(x) == 0 then
          assert(forall(l in array1d(lbound))( l <= 0) /\ forall(u in array1d(ubound))( u >= 0)
            \/ length(cover) == 0,
            "global_cardinality_low_up_closed: " ++
            "lbound and ubound must allow a count of 0 when x is empty, or also be empty",
            true)
   elseif length(cover) == 0 then
     assert(false,"global_cardinality_low_up_closed: " ++
            "cover must be empty when x is empty",
            false)
   else
     fzn_global_cardinality_low_up_closed_opt(array1d(x), array1d(cover), array1d(lbound), array1d(ubound))
   endif
  );

/** @group globals.counting
  Requires that the number of occurrences of \a cover[\p i] in \a x is \a counts[\p i].

  The elements of \a x must take their values from \a cover or be absent.
*/
predicate global_cardinality_closed(array[$X] of var set of $$E: x,
                                    array[$Y] of $$E: cover,
                                    array[$Y] of var int: counts) = 
    assert(index_sets_agree(cover, counts),
        "global_cardinality_closed: " ++
        "cover has index sets " ++ show_index_sets(cover) ++ " and count has index sets " ++ show_index_sets(counts) ++ ", but they must have identical index sets",
        if length(x) == 0 then
          forall(c in array1d(counts))(c = 0)
        else
          fzn_global_cardinality_closed_set(array1d(x), array1d(cover), array1d(counts))
        endif
    );

/** @group globals.counting
  Requires that for all \p i, the value \a cover[\p i] appears at least \a lbound[\p i]
  and at most \a ubound[\p i] times in the array \a x.

  The elements of \a x must take their values from \a cover or be absent.
*/
predicate global_cardinality_closed(array[$X] of var set of $$E: x,
                                    array[$Y] of $$E: cover,
                                    array[$Y] of int: lbound,
                                    array[$Y] of int: ubound) = 
  assert(
   index_sets_agree(cover,lbound) /\ index_sets_agree(cover,ubound),
   "global_cardinality_low_up_closed: " ++
   "cover has index sets " ++ show_index_sets(cover) ++ ", lbound has index sets " ++ show_index_sets(lbound) ++ ", and ubound has index sets " ++ show_index_sets(lbound) ++ ", but they must have identical index sets",
   if length(x) == 0 then
          assert(forall(l in array1d(lbound))( l <= 0) /\ forall(u in array1d(ubound))( u >= 0)
            \/ length(cover) == 0,
            "global_cardinality_low_up_closed: " ++
            "lbound and ubound must allow a count of 0 when x is empty, or also be empty",
            true)
   elseif length(cover) == 0 then
     assert(false,"global_cardinality_low_up_closed: " ++
            "cover must be empty when x is empty",
            false)
   else
     fzn_global_cardinality_low_up_closed_set(array1d(x), array1d(cover), array1d(lbound), array1d(ubound))
   endif
  );
