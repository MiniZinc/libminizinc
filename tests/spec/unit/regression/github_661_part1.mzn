/***
!Test
solvers: [gecode]
expected: !Result
  status: UNSATISFIABLE
***/
predicate foo(var int: x);
constraint foo(min({}));