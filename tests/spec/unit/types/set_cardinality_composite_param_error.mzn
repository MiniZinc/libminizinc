/***
!Test
solvers: [gecode]
expected: !Error
  type: TypeError
  regex: .*only allowed in variable declarations
***/

% set(...) cardinality syntax is not allowed inside composite function parameter types either
function var int: f(record(var set(1..3) of 1..10: a): r) = card(r.a);

solve satisfy;
