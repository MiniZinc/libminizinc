/** @group globals.counting
  Constrains \a c to be greater than or equal to the number of occurrences
  of \a y in \a x. 
*/
predicate count_geq(array[int] of var int: x, var int: y, var int: c) =
    fzn_count_geq(x,y,c);

include "fzn_count_geq.mzn";

%-----------------------------------------------------------------------------%
%-----------------------------------------------------------------------------%
