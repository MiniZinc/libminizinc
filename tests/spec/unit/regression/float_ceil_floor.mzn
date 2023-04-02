/***
!Test
check_against: []
expected:
- !Result
  solution:
  - !Solution
    c: -1
    f: -1
    p: -1.0
  status: ALL_SOLUTIONS
extra_files: []
markers: []
name: ''
options:
  all_solutions: true
solvers:
- gecode
type: solve
***/

var -1.75..1.75: p;
var -2..2: f;
var -2..2: c;

constraint f = floor(p) /\ c = ceil(p) /\ p = -1.0;
solve satisfy;
