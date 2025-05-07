/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
***/

array [1..2] of bool: x = [false, true];
array [1..2] of var 1..1: v;
constraint exists (i in 1..2) (not x[v[i]]);
