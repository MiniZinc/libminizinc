/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
***/

var int: x;
var int: y;
var int: z;

constraint z = x / y;
