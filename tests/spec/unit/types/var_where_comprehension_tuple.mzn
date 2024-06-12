/***
!Test
solvers: [gecode]
expected: !Error
  type: TypeError
***/

var bool: b;
any: x = [(1,) | i in 1..1 where b];
