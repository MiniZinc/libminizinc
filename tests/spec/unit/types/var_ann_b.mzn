/***
!Test
check_against: []
expected:
- !Error
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
constraint b :: if b then domain else bounds endif;