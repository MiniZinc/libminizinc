/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    a: 13
    b: 8
***/

% Defaults compose with named arguments: `a` binds only the required `x` by
% name and lets `y` default; `b` overrides the default by name.

function int: foo(int: x, int: y = 10) = x + y;

var 0..100: a;
var 0..100: b;
constraint a = foo(x: 3);
constraint b = foo(x: 3, y: 5);

solve satisfy;
