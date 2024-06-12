/***
!Test
solvers: [gecode]
expected: !Error
  type: TypeError
***/

annotation foo;
var 1..1: i;
array [int] of tuple(ann): x = [(foo,)];
any: y = x[i];
