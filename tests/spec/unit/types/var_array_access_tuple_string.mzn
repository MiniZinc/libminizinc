/***
!Test
solvers: [gecode]
expected: !Error
  type: TypeError
***/

var 1..1: i;
array [int] of tuple(string): x = [("foo",)];
any: y = x[i];
