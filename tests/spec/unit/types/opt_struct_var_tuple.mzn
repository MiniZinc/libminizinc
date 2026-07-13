/***
!Test
solvers: [gecode]
expected: !Error
  type: TypeError
type: compile
***/

% An optional struct is either the whole struct or absent; there is no var encoding of that,
% so `var opt' structs are rejected.
var opt tuple(int, int): a;
solve satisfy;
