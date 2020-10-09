/***
!Test
solvers: [gecode, chuffed]
options:
  all_solutions: true
expected: !Result
  solution: !SolutionSet
  - !Solution
    x: 1
  - !Solution
    x: 3
  - !Solution
    x: null
***/

% Regression test for issue where the incorrect solution x = 2 would be produced

var opt {1, 3}: x;

solve satisfy;