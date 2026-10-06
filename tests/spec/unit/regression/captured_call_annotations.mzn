/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    x: 1
***/

% The captured annotations of a call include annotation calls without a body. Flattening such an
% annotation creates a new call that nothing else holds, so the array of captured annotations has to
% keep it alive.

annotation my_ann(var int: v);

predicate my_pred(var int: x, var int: y) ann: preds =
  if my_ann(x) in preds /\ my_ann(y) in preds then x = 1 else x = 2 endif;

var 0..10: x;
var 0..10: y;

constraint my_pred(x, y) :: my_ann(y) :: my_ann(x);
