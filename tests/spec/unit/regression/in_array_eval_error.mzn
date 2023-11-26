/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    x: [1]
    y: true
***/

% Regression test for incorrect evaluation error due to assuming `in` would have
% a set for its RHS

array [1..1] of var 1..1: x;
var bool: y :: output = x[1] in [1];
