/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
***/

array[1..2] of var bool: x;
constraint count_geq(enum2int(x), false, 1);
