/***
!Test
solvers: [gecode]
expected: !Error
  type: TypeError
type: compile
***/

% A field of an optional struct is optional, but there is no such thing as an absent array, so
% an array-typed field of an optional struct cannot be accessed directly.
opt record(array [1..2] of int: a): r = <>;
output ["\(r.a)"];
