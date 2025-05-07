/***
!Test
solver: [gecode]
expected: !Result
  solution: !Solution
    x: true
***/

function var opt bool: foo() = true;
any: x :: output = foo();
