/***
!Test
solvers: [gecode]
expected: !Error
  type: TypeError
  regex: "named arguments are not .* supported for builtin `int_lin_eq'"
type: compile
***/

% Named arguments are not (yet) supported for low-level builtins: a builtin's
% overload family is anchored by a body-less declaration and is positional-only
% in this release. `int_lin_eq` is a body-less FlatZinc builtin, so a named call
% to it is rejected with a clear message (rather than a confusing "no parameter
% named ..." that a solver-library renaming could otherwise produce).

array [1..2] of var int: a;
constraint int_lin_eq(as: [1, 1], xs: a, y: 5);

solve satisfy;
