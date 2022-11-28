/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    x: 1
    y: 3
***/

% Ensure mkPar works correctly for tuples

function var int: foo(tuple(var int, var int): z) = z.1 + z.2;
var 1..3: x;
var 1..3: y;
constraint foo((x, y)) = 4;
constraint x < y;
