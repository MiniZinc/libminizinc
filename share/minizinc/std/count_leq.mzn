include "fzn_count_leq.mzn";

/** @group globals.counting
  Constrains \a c to be less than or equal to the number of occurrences
  of \a y in \a x. 
*/
predicate count_leq(array[int] of var int: x, var int: y, var int: c) =
    fzn_count_leq(x,y,c);

%-----------------------------------------------------------------------------%
