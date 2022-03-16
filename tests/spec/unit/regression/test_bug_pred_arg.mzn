/***
!Test
check_against: []
expected:
- !Result
  solution:
  - !Solution
    b: false
    x: 1
  - !Solution
    b: true
    x: 1
  - !Solution
    b: false
    x: 2
  status: ALL_SOLUTIONS
extra_files: []
markers: []
name: ''
options:
  all_solutions: true
solvers: [gecode]
type: solve
***/

var bool: b;
var 1..2: x;
predicate foo(var 1..1: a) = true;
constraint b -> foo(x);
