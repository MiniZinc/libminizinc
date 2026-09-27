/***
!Test
solvers: [gecode]
options:
  all_solutions: true
  -G: linear
expected: !Result
  solution: !SolutionSet
  - !Solution
    sx: !!set {}
    sy: !!set {1}
    ge: false
    rg: false
  - !Solution
    sx: !!set {1}
    sy: !!set {1}
    ge: true
    rg: true
  - !Solution
    sx: !!set {1}
    sy: !!set {}
    ge: true
    rg: true
  - !Solution
    sx: !!set {}
    sy: !!set {}
    ge: true
    rg: true
  status: ALL_SOLUTIONS
***/

% An output variable that reaches a reverse-mapped variable through a chain of aliases (here
% via the function call and common subexpression elimination) must be defined by the reverse
% mapper. With the linear library, rg used to be left unconstrained in the FlatZinc.
function var set of int: dv(var set of int: a) :: promise_total =
  let { var set of ub(a): r; constraint r = a; } in r;
function var bool: myge(var set of int: a, var set of int: b) = dv(a) >= dv(b);
var set of 1..1: sx;
var set of 1..1: sy;
var bool: ge :: output = myge(sx, sy);
var bool: rg :: output = dv(sx) >= dv(sy);
