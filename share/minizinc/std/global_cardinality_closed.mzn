include "fzn_global_cardinality_closed.mzn";
include "fzn_global_cardinality_closed_reif.mzn";

/** @group globals.counting
  Requires that the number of occurrences of \a cover[\p i] in \a x is \a counts[\p i].

  The elements of \a x must take their values from \a cover.
*/
predicate global_cardinality_closed(array[$X] of var int: x,
                                    array[int] of int: cover,
                                    array[int] of var int: counts) =
    assert(index_set(cover) = index_set(counts),
        "global_cardinality_closed: " ++
        "cover and counts must have identical index sets",
        fzn_global_cardinality_closed(array1d(x), cover, counts)
    );
