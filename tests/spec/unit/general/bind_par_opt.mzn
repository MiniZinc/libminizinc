/***
!Test
solvers: [gecode]
expected: !Result
  status: UNSATISFIABLE
***/

array [1..1] of var opt 1..4: x;
constraint x[1] = 2;
constraint x[1] = 1;

array [1..1] of var opt 1.0..4.0: y;
constraint y[1] = 2.0;
constraint y[1] = 1.0;
