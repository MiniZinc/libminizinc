/***
!Test
check_against: []
expected:
- !Result
  solution: !Solution {}
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

test f(5..8: x, int: y) = x + y <= 4;
constraint f(3,0) \/ true;
