/***
!Test
solvers: [gecode]
expected: !Error
  regex: .*array access out of bounds, array `x' is empty.*
***/

array [int] of int: x = [];
any: v = x[1];
