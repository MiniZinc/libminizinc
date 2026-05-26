/***
!Test
solvers: [gecode]
type: compile
expected: !FlatZinc named_args_poly.fzn
***/

% A single polymorphic predicate called with named arguments. The
% Instantiator generates a monomorphic instance keyed on the concrete
% types (int, int); the named-args resolution composes cleanly with
% type_specialise so the body is correctly inlined.

predicate q(var $$T: x, var $$T: y) =
  x + y = 13;

var 1..10: a;
var 1..10: b;

constraint q(x: a, y: b);

solve satisfy;
