/***
!Test
check_against: []
expected:
- !Result
  solution: !Solution
    y:
      c: C
      e: 2
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

set of int: XYZ = 2..10;
enum Y = C(XYZ);
var Y: y;
