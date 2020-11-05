include "fzn_global_cardinality.mzn";
include "fzn_global_cardinality_reif.mzn";

/** @group globals.counting
  Requires that the number of occurrences of \a cover[\p i] in \a x is \a counts[\p i].
*/
predicate global_cardinality(array[$X] of var int: x,
                             array[$Y] of int: cover,
                             array[$Y] of var int: counts) =
    assert(index_sets_agree(cover, counts),
        "global_cardinality: cover and counts must have identical index sets",
        fzn_global_cardinality(array1d(x), array1d(cover), array1d(counts))
    );
