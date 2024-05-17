/***
!Test
solvers: [gecode]
options:
  all_solutions: true
expected: !Result
  status: ALL_SOLUTIONS
  solution:
  - !Solution
    x: 2
***/

var 1..2: x;
constraint not let {
  constraint x = 1;
} in true;
