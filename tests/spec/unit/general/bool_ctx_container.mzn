/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
***/

function array [int] of var bool: foo(array [int] of tuple(var bool): x) = x.1;
array [int] of var bool: c = array1d(1..3, foo([let { var bool: b; } in (b,) | i in 1..3]));
