/***
!Test
solvers: [gecode]
expected: !Error
  type: TypeError
***/

annotation foo;
var 1..1: i;
array [int] of ann: x = [foo];
any: y = x[i];
