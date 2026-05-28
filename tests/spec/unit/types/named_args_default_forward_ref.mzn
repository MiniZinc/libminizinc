/***
!Test
solvers: [gecode]
expected: !Error
  type: TypeError
  regex: "undefined identifier `x'"
type: compile
***/

% v1 restriction: a default expression may not reference another parameter.
% Default expressions are typechecked in the enclosing scope, where the other
% parameters are not visible, so the reference to `x` is undefined.

function int: foo(int: x, int: y = x + 1) = x + y;
constraint foo(3) = 7;

solve satisfy;
