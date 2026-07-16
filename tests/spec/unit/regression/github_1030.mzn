/***
!Test
expected: !Result
  status: SATISFIED
***/

% Used to produce invalid FlatZinc with a false boolean literal in an integer array literal
% due to default operator not properly folowing IDs, so the LHS is not found to be par
% and deopt(<>) was called (giving a false positive warning about undefinedness, and returning false).

constraint xy = [1, <>];

include "globals.mzn";

array[1..2] of var opt 1..4: xy;

constraint 
  let {
    array [int] of var int: xx = [xy[i] default 0 | i in index_set(xy)];
  } in all_different(xx, except: {0});
