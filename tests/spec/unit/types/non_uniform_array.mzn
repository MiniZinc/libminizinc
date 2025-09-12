/***
!Test
solvers: [gecode]
expected: !Error
  type: TypeError
***/

array [int] of tuple(set of int): x :: output = [({1},), (2,)];
