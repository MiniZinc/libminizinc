include "fzn_global_cardinality.mzn";

/** @group globals.counting
  Requires that the number of occurrences of \a cover[\p i] in \a x is \a counts[\p i].
*/
predicate global_cardinality(array[int] of var int: x,
	  		     array[int] of int: cover,
                             array[int] of var int: counts) =
    assert(index_set(cover) = index_set(counts),
        "global_cardinality: cover and counts must have identical index sets",
    fzn_global_cardinality(x, cover, counts)
    );

/** @group globals.counting
  Returns the number of occurrences of \a cover[\p i] in \a x.
*/
function array[int] of var int: global_cardinality(array[int] of var int: x,
	  		                           array[int] of int: cover) :: promise_total =
  let { array[index_set(cover)] of var 0..length(x): counts;
        constraint fzn_global_cardinality(x,cover,counts) } in
  counts;
