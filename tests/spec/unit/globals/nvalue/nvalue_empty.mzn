/***
!Test
solvers: [chuffed]
expected: !Result
  solution: !Solution
***/

include "nvalue.mzn";
var bool: b ::output = nvalue(3, []);
constraint nvalue(0, []);
