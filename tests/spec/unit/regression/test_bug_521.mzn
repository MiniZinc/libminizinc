/***
!Test
check_against: []
expected:
- !Result
  solution: !Solution
    x: 0
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

var 0..1: x;
var opt bool: b = if x = 1 then true elseif false then <> else true endif;
