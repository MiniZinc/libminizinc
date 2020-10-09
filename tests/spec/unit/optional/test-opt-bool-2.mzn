/***
!Test
solvers: [gecode, chuffed]
options:
  all_solutions: true
expected: !Result
  solution: !SolutionSet
  - !Solution
    x: true
    y: true
  - !Solution
    x: true
    y: null
  - !Solution
    x: null
    y: true
  - !Solution
    x: null
    y: null
  status: ALL_SOLUTIONS
***/

var opt bool: x;
var opt bool: y;

constraint x /\ y;