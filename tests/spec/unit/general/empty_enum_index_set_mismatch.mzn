/***
!Test
solvers: [gecode]
expected: !Error
  type: EvaluationError
***/

enum E = A(1..1);
array[E, int] of int: x = [
  (i, j): 1 | i in E, j in 1..0
];
