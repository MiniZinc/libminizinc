/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    a: 11
    b: 3
***/

% A parameter with a `default` may be omitted at the call site; the default
% expression is spliced into the argument list during typechecking. `a` omits
% the defaulted `y` (so y = 10); `b` supplies it positionally.

function int: foo(int: x, int: y = 10) = x + y;

var 0..100: a;
var 0..100: b;
constraint a = foo(1);
constraint b = foo(1, 2);

solve satisfy;
