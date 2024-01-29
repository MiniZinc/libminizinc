/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    x: 3
***/

annotation foo_ann(var int: x :: annotated_expression, int: a, int: b) = let {
    constraint a * x + b = 10;
} in empty_annotation;

var 1..10: x :: foo_ann(3, 1);
