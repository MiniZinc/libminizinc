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

var 1..10: x;
annotation fooann(var int: x);
constraint true ::fooann(x);
