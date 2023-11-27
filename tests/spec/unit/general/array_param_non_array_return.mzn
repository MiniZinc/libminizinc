/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    x: [1]
***/

function any $T: foo(array [int] of any $T: x) = x[1];
any: x :: output = foo([(1,)]);
