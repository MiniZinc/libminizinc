/***
!Test
solvers: [gecode]
expected: !Error
  type: TypeError
  regex: 'no function or predicate with this signature found'
type: compile
***/

% A named argument whose name does not match any parameter must produce a
% TypeError. The error tells the user which signatures are compatible with
% the supplied positional/named names.

predicate foo(int: x, int: y);
constraint foo(x: 1, z: 2);

solve satisfy;
