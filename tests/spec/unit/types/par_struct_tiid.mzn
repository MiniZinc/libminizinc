/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
***/

function $T: foo(var $T: a, $T: b) = b;

tuple(var 1..1): x;
tuple(int): y = (1,);
tuple(int): z = foo(x, y);
