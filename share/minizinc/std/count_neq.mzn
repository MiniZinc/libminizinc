include "fzn_count_neq.mzn";

/** @group globals.counting
  Constrains \a c to be not equal to the number of occurrences
  of \a y in \a x. 
*/
predicate count_neq(array[int] of var int: x, var int: y, var int: c) =
    fzn_count_neq(x,y,c);

%-----------------------------------------------------------------------------%
