/***
!Test
solvers: [gecode]
expected: !Result
  status: UNSATISFIABLE
***/

function tuple(var 1..2): foo() = (3,);
any: x = foo();
