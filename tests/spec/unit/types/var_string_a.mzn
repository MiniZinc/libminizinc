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
var string: s = if b then "foo" else "bar" endif;