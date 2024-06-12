/***
!Test
solvers: [gecode]
expected: !Error
  type: TypeError
***/

var set of 1..1: s;
any: x = [(1,) | i in s];
