/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    x: [null]
***/
function array [int] of opt 1..3: foo() = [[1, <>][2]];
any: x :: output = foo();
