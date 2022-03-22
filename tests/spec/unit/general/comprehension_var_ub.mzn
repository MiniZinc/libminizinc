/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
***/

var set of 1..3: x;
function var set of int: foo() = x;
constraint forall (i in foo()) (true);
