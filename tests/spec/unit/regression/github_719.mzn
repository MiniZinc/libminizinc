/***
!Test
solvers: [gecode]
options:
  all_solutions: true
expected: !Result
  status: ALL_SOLUTIONS
  solution:
  - !Solution
    foo: 1
***/

var 1..2: foo;
constraint 9 <= [10, 0][(foo > 1) + 1];
