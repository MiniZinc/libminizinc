/***
!Test
solvers: [gecode]
expected: !Error
  regex: .*array access out of bounds.*
***/

array [int] of tuple(int): x = [(1,)];
any: a = x[2];
