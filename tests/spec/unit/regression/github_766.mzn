/***
!Test
expected: !Result
  status: ALL_SOLUTIONS
  solution: !SolutionSet
  - !Solution
    a: -2
    b: -2
  - !Solution
    a: -2
    b: 2
  - !Solution
    a: 2
    b: -2
  - !Solution
    a: 2
    b: 2
options:
  all_solutions: true
solvers: [gecode]
***/
var {-2,2}: a;
var {-2,2}: b;
constraint ((((b - a) ^ -1)::mzn_break_here * 3) = 3) <-> false;
