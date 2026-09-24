/***
!Test
solvers: [gecode, chuffed]
options:
  all_solutions: true
expected: !Result
  solution: !SolutionSet
  - !Solution
    k: 1
    x: null
  - !Solution
    k: 1
    x: !!set {1}
  - !Solution
    k: 1
    x: !!set {2}
  - !Solution
    k: 2
    x: null
  - !Solution
    k: 2
    x: !!set {1, 2}
  status: ALL_SOLUTIONS
***/

% The cardinality in var opt set(k) of S only applies if the set occurs
var 1..2: k;
var opt set(k) of 1..2: x;
