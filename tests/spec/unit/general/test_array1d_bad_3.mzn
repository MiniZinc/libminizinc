/***
!Test
expected: !Error
  type: EvaluationError
***/

array [1..3] of int: a = array1d(1..3, [1, 2, 3, 4]);

solve satisfy;
