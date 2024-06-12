/***
!Test
solvers: [gecode]
expected: !Error
  type: TypeError
***/

var 1..1: i;
array [int] of string: x = ["foo"];
any: y = x[i];
