include "fzn_global_cardinality_closed.mzn";
include "fzn_global_cardinality_closed_reif.mzn";

/** @group globals.counting
  Requires that the number of occurrences of \a cover[\p i] in \a x is \a counts[\p i].

  The elements of \a x must take their values from \a cover.
*/
predicate global_cardinality_closed(array[$X] of var int: x,
                                    array[$Y] of int: cover,
                                    array[$Y] of var int: counts) =
    assert(index_sets_agree(cover, counts),
        "global_cardinality_closed: " ++
        "cover and counts must have identical index sets",
        fzn_global_cardinality_closed(array1d(x), array1d(cover), array1d(counts))
    );
