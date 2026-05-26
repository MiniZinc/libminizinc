/***
!Test
solvers: [gecode]
type: compile
expected: !FlatZinc named_args_overload_disambig.fzn
***/

% Named-argument syntax disambiguates between overloads with different
% parameter types. Each call resolves to the overload whose parameter
% names match the supplied names; the FlatZinc shows the matched
% predicate id and the arguments in matched-decl parameter order.

predicate foo(int: x, int: y);
predicate foo(int: a, bool: c);

constraint foo(x: 1, y: 2);
constraint foo(a: 3, c: true);

solve satisfy;
