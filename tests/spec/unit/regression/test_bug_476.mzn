/***
!Test
check_against: []
expected:
- !Result
  solution: !Solution
    inner:
      inner:
        b: 1
        c: true
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

var 1..9: b;
var bool: c;
constraint b = c;
solve satisfy;