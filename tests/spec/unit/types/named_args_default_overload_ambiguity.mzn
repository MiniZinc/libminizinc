/***
!Test
solvers: [gecode]
expected: !Error
  type: TypeError
  regex: "ambiguous overloading"
type: compile
***/

% Two overloads where one is a default-extension of the other are forbidden:
% a call like `foo(5)` could match either body, silently. The user must give
% the longer overload a different name.

function int: foo(int: x) = x;
function int: foo(int: x, int: y = 0) = x + y;
constraint foo(5) = 5;

solve satisfy;
