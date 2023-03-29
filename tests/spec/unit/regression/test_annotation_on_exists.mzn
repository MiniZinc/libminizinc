/***
!Test
check_against: []
expected:
- !Result
  solution:
  - !Solution
    b: true
    c: true
    d: false
  - !Solution
    b: true
    c: false
    d: true
  - !Solution
    b: true
    c: true
    d: true
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

var bool: b;
annotation remove(var bool: b);
var bool: c;
var bool: d;
constraint (c \/ d )::remove(b);
constraint b;
