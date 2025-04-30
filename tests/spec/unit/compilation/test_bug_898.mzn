/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
***/
array[1..1] of var opt 0..10: y;
constraint y = [1];
constraint y[1] <= 4;
