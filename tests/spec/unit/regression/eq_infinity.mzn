/***
!Test
solvers: [gecode]
options:
  all_solutions: true
expected: !Result
  solution: !SolutionSet
  - !Solution
    x: 1
    b: true
  - !Solution
    x: 2
    b: true
***/

var 1..2: x;
var bool: b;
constraint b \/ x=-infinity;

