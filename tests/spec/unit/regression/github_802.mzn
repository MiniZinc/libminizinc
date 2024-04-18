/***
!Test
solvers: [gecode]
options:
  all_solutions: true
  keep-paths: true
expected: !Result
  status: ALL_SOLUTIONS
  solution: !SolutionSet
  - !Solution
    i: 1
    x: 5
  - !Solution
    i: 2
    x: 10
***/

% Previously the generated array accesses for a[i].a and a[i].b had the same
% paths so would get incorrectly unified.

any: a = [(a: 16, b: 2), (a: 8, b: 1)];
var 1..2: i;
any: x :: output = 10 div a[i].b;
