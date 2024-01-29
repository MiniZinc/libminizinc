/***
!Test
solvers: [gecode]
options:
  all_solutions: true
expected: !Result
  solution: !SolutionSet
  - !Solution
    x: [2, null, null]
  - !Solution
    x: [null, 2, null]
  - !Solution
    x: [1, 1, null]
  - !Solution
    x: [null, null, 2]
  - !Solution
    x: [1, null, 1]
  - !Solution
    x: [null, 1, 1]
  status: ALL_SOLUTIONS
***/

array [1..3] of var opt 1..5: x;

constraint sum(x) = 2;
