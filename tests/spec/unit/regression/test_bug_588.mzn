/***
!Test
check_against: []
expected:
- !Result
  solution: !Solution
    x: 2
    y: 1
    z: 1
  status: SATISFIED
extra_files: []
markers: []
name: ''
options:
  all_solutions: false
solvers: [gecode]
type: solve
***/

function var opt int: m(var opt int: x, var opt int: y) = x - y;
var 1..3: x;
var 1..3: y;
var 1..3: z;
constraint z = m(x, y);
