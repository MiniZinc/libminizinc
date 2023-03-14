/***
!Test
solvers: [gecode]
options:
  all_solutions: true
expected: !Result
  solution: !SolutionSet
  - !Solution
    x: -1
    y: -1
    p: -1
    q: -1
  - !Solution
    x: -1
    y: 0
    p: -1
    q: 1
  - !Solution
    x: 0
    y: -1
    p: 42
    q: 42
  - !Solution
    x: 0
    y: 0
    p: 42
    q: 1
***/

% Previously, incorrect domain in pow[_fixed}_mt made 0^-1 cause UNSAT rather than return undefined

var -1..0: x;
var -1..0: y;

var int: p :: output = pow(x, -1) default 42;
var int: q :: output = pow(x, y) default 42;
