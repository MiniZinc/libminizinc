/***
!Test
expected: !Result
  status: SATISFIED
***/

include "globals.mzn";

function array[int] of $T: mrow(array[int, int] of $T: x, int: r) =
  [x[r,i] | i in index_set_2of2(x)];

constraint all_different(mrow([|1,2|2,3|],1));

solve satisfy;