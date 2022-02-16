/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    y: 0
***/

% Regression test for bug #532 (infinite recursion in create_output).

function 0..0: f(1..1: x) = if true then 0 else f(1) endif;
var 0..0: y :: output = f(1);
