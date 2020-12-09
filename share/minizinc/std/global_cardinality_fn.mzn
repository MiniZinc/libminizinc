include "global_cardinality.mzn";

/** @group globals.counting
  Returns the number of occurrences of \a cover[\p i] in \a x.
*/
function array[$Y] of var int: global_cardinality(array[$X] of var int: x,
                                                  array[$Y] of int: cover) :: promise_total =
  let { array[int] of int: cover1d = array1d(cover);
        array[index_set(cover1d)] of var 0..length(x): counts;
        constraint global_cardinality(array1d(x),array1d(cover),array1d(counts)) } in
  arrayXd(cover,counts);
