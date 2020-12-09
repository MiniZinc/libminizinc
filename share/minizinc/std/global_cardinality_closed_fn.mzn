include "global_cardinality_closed.mzn";

/** @group globals.counting
  Returns an array with number of occurrences of \a cover[\p i] in \a x.

  The elements of \a x must take their values from \a cover.
*/
function array[$Y] of var int: global_cardinality_closed(array[$X] of var int: x,
                                                         array[$Y] of int: cover) :: promise_total =
  let { array[int] of int: cover1d = array1d(cover);
        array[index_set(cover1d)] of var 0..length(x): counts;
        constraint global_cardinality_closed(array1d(x),cover1d,counts); } 
  in arrayXd(cover,counts);
