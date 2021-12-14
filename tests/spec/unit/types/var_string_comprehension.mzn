/***
!Test
check_against: []
expected: !Error
    type: TypeError
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
constraint trace("\(["lala" | i in 1..3 where b])");