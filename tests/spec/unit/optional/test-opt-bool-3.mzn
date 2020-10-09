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
    x: false
    y: true
  - !Solution
    x: true
    y: false
  status: ALL_SOLUTIONS
***/

var opt bool: x;
var opt bool: y;

constraint x \/ y;