/***
!Test
check_against: []
expected:
- !Result
  solution: !Solution
    x: 1
  status: SATISFIED
extra_files: []
markers: []
name: ''
options:
  all_solutions: false
solvers:
- gecode
type: solve
***/
var opt 0..1: x;
constraint count (i in 1..1) (x = 1) = 1;
