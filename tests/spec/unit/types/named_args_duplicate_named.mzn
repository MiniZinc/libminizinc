/***
!Test
solvers: [gecode]
expected: !Error
  type: TypeError
  regex: 'named argument .* given more than once'
type: compile
***/

% The same parameter name may not appear more than once among the named
% arguments of a single call.

predicate foo(int: x, int: y);
constraint foo(x: 1, x: 2);

solve satisfy;
