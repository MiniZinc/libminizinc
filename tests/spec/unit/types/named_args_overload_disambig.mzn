/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    r1: 1
    r2: 2
***/

% Named-argument syntax disambiguates between overloads with different
% parameter types. Each call resolves to the overload whose parameter names
% match the supplied names. `foo` is a user function (named arguments are not
% available for positional-only builtins); each body returns a distinct value
% so the solution reveals which overload each named call selected.

function int: foo(int: x, int: y) = 1;
function int: foo(int: a, bool: c) = 2;

var 0..9: r1;
var 0..9: r2;
constraint r1 = foo(x: 1, y: 2);
constraint r2 = foo(a: 3, c: true);

solve satisfy;
