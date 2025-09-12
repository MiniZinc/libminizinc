/***
!Test
solvers: [gecode]
options:
  all_solutions: true
expected: !Result
  solution: !SolutionSet
  - !Solution
    x: [1, 1]
  - !Solution
    x: [2, 1]
  - !Solution
    x: [1, 2]
***/
array[1..2] of var 1..3: x;
var 1..3: z = sum(x);
