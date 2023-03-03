/***
!Test
check_against: []
expected:
- !Result
  solution: !Solution
    b: true
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

var bool: b;
opt int: abs = <>;
constraint b \/ deopt(abs) = 1;
