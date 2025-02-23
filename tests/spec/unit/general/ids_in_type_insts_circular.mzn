/***
!Test
solvers: [gecode]
expected: !Error
  type: TypeError
***/

function set of int: foo(1..A: x) = 1..x;
foo(3): A = 1;
