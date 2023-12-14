/***
!Test
expected: !Result
  status: UNSATISFIABLE
solvers: [gecode]
***/
var 1..1: x;
var 1..1: y :: output = if false then x else 0 endif;
