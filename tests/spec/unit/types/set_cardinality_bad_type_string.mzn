/***
!Test
solvers: [gecode]
expected: !Error
  type: TypeError
  regex: .*cardinality expression must have type
***/

% Cardinality expression has unsupported type `string'
var set("foo") of 1..10: x;

solve satisfy;
