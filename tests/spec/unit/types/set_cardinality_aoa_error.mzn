/***
!Test
solvers: [gecode]
expected: !Error
  type: TypeError
  regex: .*nested
***/

% set(...) cardinality syntax is not supported for nested array-of-array types
array[1..2] of array[1..2] of var set(1..2) of 1..5: x;

solve satisfy;
