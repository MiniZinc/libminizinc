/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    p: true
    q: true
***/

% Regression test: an annotation that captures the annotated expression
% (:: annotated_expression) on a `var` array declaration that also has a
% right-hand side used to crash the flattener with "not supported yet". The
% annotation was flattened (materialising element variables as the array's RHS)
% before the real RHS was bound.

annotation force_true(array [$T] of var bool: b :: annotated_expression) = let {
    constraint forall (x in array1d(b)) (x);
} in empty_annotation;

var bool: p;
var bool: q;
array [1..2] of var bool: x :: force_true = [p, q];
