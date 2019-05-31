/** @group globals.counting
  Constrains \a c to be strictly less than the number of occurrences
  of \a y in \a x. 
*/
predicate count_lt(array[int] of var int: x, var int: y, var int: c) =
    fzn_count_lt(x,y,c);

include "fzn_count_lt.mzn";

%-----------------------------------------------------------------------------%
%-----------------------------------------------------------------------------%
