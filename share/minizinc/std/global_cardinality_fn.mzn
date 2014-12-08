include "global_cardinality.mzn";

/** @group globals.counting
  Returns the number of occurrences of \a cover[\p i] in \a x.
*/
function array[int] of var int: global_cardinality(array[int] of var int: x,
	  		                           array[int] of int: cover) :: promise_total =
  let { array[index_set(cover)] of var 0..length(x): counts;
        constraint global_cardinality(x,cover,counts) } in
  counts;
