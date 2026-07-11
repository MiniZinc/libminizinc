/***
!Test
solvers: [gecode]
expected: !Error
  type: TypeError
  regex: "ambiguous overloading"
type: compile
***/

% Parameter defaults let two declarations be ambiguous under named arguments even though
% they have different arities and no common prefix of parameter types: `z` never has to be
% supplied, so `foo(x: 1, y: 2.0)` supplies exactly `x` and `y` in both, at the same types,
% and matches both declarations.

predicate foo(int: x, float: y);
predicate foo(float: y, int: x, int: z = 0);

constraint foo(x: 1, y: 2.0);

solve satisfy;
