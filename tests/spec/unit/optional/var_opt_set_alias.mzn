/***
!Test
solvers: [gecode, cp-sat]
options:
  all_solutions: true
expected: !Result
  solution: !SolutionSet
  - !Solution
    v: 1
    x: !!set {1}
  - !Solution
    v: 2
    x: !!set {2}
  status: ALL_SOLUTIONS
***/

% An alias does not need a declared upper bound
var 1..2: v;
var opt set of int: x :: output = {v};
