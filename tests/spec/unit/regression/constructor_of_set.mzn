/***
!Test
check_against: []
expected:
- !Result
  solution: !Solution
    y:
      c: C
      e: 1
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

set of int: XYZ = 1..10;
enum Y = C(XYZ);
var Y: y;
