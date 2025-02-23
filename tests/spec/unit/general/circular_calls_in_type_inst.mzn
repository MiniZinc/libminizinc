/***
!Test
solvers: [gecode]
expected: !Error
  type: TypeError
***/

function set of int: foo(bar(1): x) = 1..3;
function set of int: bar(foo(1): x) = 1..3;
