/***
!Test
check_against: []
expected:
- !Result
  solution: !Solution
    x: 0
extra_files: []
markers: []
name: ''
options:
  all_solutions: false
solvers:
- gecode
- chuffed
- cbc
type: solve
***/

var 0..1: x;
solve minimize -1 + 2*x;
