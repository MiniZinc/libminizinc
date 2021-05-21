/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution

***/

% Regression test for crash when flattening multiplication/division binop
% involving array comprehension.

int: N = 1000;

array [1..N] of var 1..N: x;
var 1..N: y;

constraint forall (n in 1..N) (n = sum ([x[i] | i in 1..n]) * y);
