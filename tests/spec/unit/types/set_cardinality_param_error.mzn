/***
!Test
solvers: [gecode]
expected: !Error
  type: TypeError
  regex: .*only allowed in variable declarations
***/

% set(...) cardinality syntax is not allowed in function parameters
function var int: f(set(1..3) of 1..10: p) = 1;

solve satisfy;
