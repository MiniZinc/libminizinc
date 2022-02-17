/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    x: []
***/

% Regression test for bug #534 (fail on empty domain even for empty array).

array [1..2, 1..0] of var 1..0: x;
