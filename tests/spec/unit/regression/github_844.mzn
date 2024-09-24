/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    x: [[]]
    y: [[]]
***/

set of int: rows = 1..1;
set of int: cols = 1..0;
array[rows, cols] of var int: y ::output = [];
array[rows, cols] of par int: x ::output = [||];