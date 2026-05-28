/***
!Test
solvers: [gecode]
expected: !Error
  type: TypeError
  regex: "parameter .* without a default follows a parameter with a default"
type: compile
***/

% A parameter without a default may not follow one that has a default: such a
% default could never be omitted at a call and would only be confusing.

function int: foo(int: x = 0, int: y) = x + y;
constraint foo(1, 2) = 3;

solve satisfy;
