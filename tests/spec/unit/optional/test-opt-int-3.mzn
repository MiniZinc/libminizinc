/***
!Test
solvers: [gecode]
options:
  all_solutions: true
expected: !Result
  solution: !SolutionSet
  - !Solution
    x: [3, 2, null]
  - !Solution
    x: [2, 3, null]
  - !Solution
    x: [3, null, 2]
  - !Solution
    x: [2, null, 3]
  - !Solution
    x: [null, 3, 2]
  - !Solution
    x: [null, 2, 3]
  - !Solution
    x: [3, 2, 1]
  - !Solution
    x: [2, 3, 1]
  - !Solution
    x: [3, 1, 2]
  - !Solution
    x: [1, 3, 2]
  - !Solution
    x: [2, 1, 3]
  - !Solution
    x: [1, 2, 3]
  status: ALL_SOLUTIONS
***/

array [1..3] of var opt 1..5: x;

constraint product(x) = 6;
