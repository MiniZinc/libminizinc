/***
!Test
check_against: []
expected: !Error
    type: MiniZincTypeError
extra_files: []
markers: []
name: ''
options:
  all_solutions: false
solvers:
- gecode
type: compile
***/

var bool: b;
constraint trace("\([{1.0} | i in 1..3 where b])");