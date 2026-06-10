/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    p: true
    q: true
***/

% Regression test: an annotation capturing the annotated expression
% (:: annotated_expression) on a `var` array declared inside a `let` (with a
% right-hand side) used to crash the flattener with "not supported yet" when the
% array was lifted to the top level. The annotation materialised element
% variables as the array's RHS before the real RHS was bound.

annotation force_true(array [$T] of var bool: b :: annotated_expression) = let {
    constraint forall (x in array1d(b)) (x);
} in empty_annotation;

var bool: p;
var bool: q;
constraint let { array [1..2] of var bool: x :: force_true = [p, q]; } in x[1] /\ x[2];
