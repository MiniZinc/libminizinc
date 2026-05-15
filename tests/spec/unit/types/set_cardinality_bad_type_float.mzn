/***
!Test
solvers: [gecode]
expected: !Error
  type: TypeError
  regex: .*cardinality expression must have type
***/

% Cardinality expression has unsupported type `float'
var set(3.5) of 1..10: x;

solve satisfy;
