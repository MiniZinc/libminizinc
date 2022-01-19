/***
!Test
solvers: [chuffed, gecode]
options:
  all_solutions: true
expected: !Result
  solution: !SolutionSet
  - !Solution
    x: 1
***/

var opt 1..1: x;

constraint not absent(x);
