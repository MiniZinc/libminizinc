/***
!Test
solvers: [gecode]
options:
  all_solutions: true
expected: !Result
  status: ALL_SOLUTIONS
  solution: !SolutionSet
  - !Solution
    a: [[1]]
    x: [[1]]
***/

array [1..1] of tuple(var 1..2): a;
array [1..1] of tuple(var 1..1): x :: output = a;
