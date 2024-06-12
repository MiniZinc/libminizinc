/***
!Test
solvers: [gecode]
expected: !Error
  type: TypeError
***/

var 1..1: i;
array [int] of tuple(set of float): x = [({1.5},)];
any: y = x[i];
