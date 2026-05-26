/***
!Test
solvers: [gecode]
expected: !Error
  type: TypeError
  regex: 'positional argument after named argument'
type: compile
***/

% Positional arguments must come before any named argument. A positional
% slot following a named slot is a TypeError.

predicate foo(int: x, int: y, int: z);
constraint foo(1, y: 2, 3);

solve satisfy;
