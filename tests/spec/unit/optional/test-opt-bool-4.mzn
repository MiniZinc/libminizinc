/***
!Test
solvers: [gecode, chuffed]
options:
  all_solutions: true
expected: !Result
  solution: !SolutionSet
  - !Solution
    x: true
    y: false
  - !Solution
    x: false
    y: true
  - !Solution
    x: null
    y: null
  status: ALL_SOLUTIONS
***/

var opt bool: x :: add_to_output;
var opt bool: y :: add_to_output = not x;
