/***
!Test
solvers: [gecode, chuffed]
options:
  all_solutions: true
expected: !Result
  status: ALL_SOLUTIONS
  solution: !SolutionSet
  - !Solution
    x: null
  - !Solution
    x: 1
***/

var opt 1..1: x;

solve satisfy;
