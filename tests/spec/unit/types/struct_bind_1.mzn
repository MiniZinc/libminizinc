/***
!Test
solvers: [gecode]
options:
  all_solutions: true
expected: !Result
  status: ALL_SOLUTIONS
  solution: !SolutionSet
  - !Solution
    a: [1, 1]
    x: [1, 1]
***/

tuple(var 1..2, var 1..1): a;
tuple(var 1..1, var 1..2): x :: output = a;