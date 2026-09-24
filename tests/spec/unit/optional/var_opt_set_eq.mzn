/***
!Test
solvers: [gecode, cp-sat, chuffed]
options:
  all_solutions: true
expected: !Result
  solution: !SolutionSet
  - !Solution
    x: null
    y: !!set {}
  - !Solution
    x: null
    y: !!set {1}
  - !Solution
    x: !!set {}
    y: null
  - !Solution
    x: !!set {}
    y: !!set {1}
  - !Solution
    x: !!set {1}
    y: null
  - !Solution
    x: !!set {1}
    y: !!set {}
  status: ALL_SOLUTIONS
***/

% <> is only equal to <>, and is different from {}
var opt set of 1..1: x;
var opt set of 1..1: y;
constraint x != y;
