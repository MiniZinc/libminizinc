/***
!Test
solvers: [gecode]
options:
  all_solutions: true
expected: !Result
  solution: !SolutionSet
  - !Solution
    a: true
  - !Solution
    a: false
***/

var bool: a;
constraint <> -> a;
