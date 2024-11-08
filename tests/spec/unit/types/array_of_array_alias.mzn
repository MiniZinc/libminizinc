/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    x: [[1, 2, 3], [4, 5, 6], [7, 8, 9]]
***/
type T = array [1..3] of 1..9;
array [1..3] of var T: x;
include "globals.mzn";
constraint strictly_increasing (i, j in 1..3) (x[i][j]);
