/***
!Test
solvers: [cbc, chuffed]
expected: !Result
  solution: !Solution
    x: !!set {}
***/

include "nosets.mzn";
var set of {}: x;
