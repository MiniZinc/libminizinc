/***
!Test
solvers: [gecode]
expected: !Result
  status: UNSATISFIABLE
***/

predicate foo(tuple(1..3): x) = true;
constraint foo((4,));
