/***
!Test
solvers: [gecode]
expected: !Result
  status: UNSATISFIABLE
***/

array [1..1] of var bool: x;
constraint exists(x[..]);
constraint not exists(x[..]);
