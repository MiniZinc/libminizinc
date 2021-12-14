/***
!Test
check_against: []
expected:
- !Error
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
constraint trace("\(if b then "foo" else "bar" endif)");