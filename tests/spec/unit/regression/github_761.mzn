/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
***/

% Regression test for GitHub #761

function array [int] of any $T: foo(array [int] of any $T: x) = x;
any: x = foo([(a: 1)]);
