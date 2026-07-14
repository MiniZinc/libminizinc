/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    r: -88
***/

% A named-argument call must bind by name, not by position. `foo(y: 99, x: 11)`
% binds x = 11 and y = 99, not positionally (which would give x = 99, y = 11).
% `foo` is a user function with an asymmetric body so the solution reveals the
% binding: x - y = 11 - 99 = -88 (positional mis-binding would give +88).

function int: foo(int: x, int: y) = x - y;
var -1000..1000: r;
constraint r = foo(y: 99, x: 11);

solve satisfy;
