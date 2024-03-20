/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    y: 0
***/
include "nosets.mzn";
var set of 1..3: x :: no_output;
var int: y :: output = sum(i in x) (i);
solve minimize y;
