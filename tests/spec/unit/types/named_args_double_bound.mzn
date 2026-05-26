/***
!Test
solvers: [gecode]
expected: !Error
  type: TypeError
  regex: 'no function or predicate with this signature found'
type: compile
***/

% A parameter bound positionally must not also be bound by name. The first
% positional argument binds `x`; supplying `x: ...` as a named argument
% creates an unresolvable signature and is rejected.

predicate foo(int: x, int: y);
constraint foo(1, x: 2);

solve satisfy;
