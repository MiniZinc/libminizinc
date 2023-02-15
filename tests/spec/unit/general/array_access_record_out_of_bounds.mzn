/***
!Test
solvers: [gecode]
expected: !Error
  regex: .*array access out of bounds.*
***/

array [int] of record(int: a): x = [(a: 1)];
any: a = x[2];
