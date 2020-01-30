include "global_cardinality_closed.mzn";

/** @group globals.counting
  Returns an array with number of occurrences of \p i in \a x.

  The elements of \a x must take their values from \a cover.
*/
function array[int] of var int: global_cardinality_closed(array[int] of var int: x,
	  		                                  array[int] of int: cover) :: promise_total =
  let { array[index_set(cover)] of var 0..length(x): counts;
        constraint global_cardinality_closed(x,cover,counts); } 
  in counts;
