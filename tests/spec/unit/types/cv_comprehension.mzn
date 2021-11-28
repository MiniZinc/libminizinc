/***
!Test
check_against: []
expected:
- !Result
  solution: !Solution
    x: 1
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

var 1..3: x;
var int: t(var int: x, var int: y) = x+y;
constraint forall (i in 1..ub(t(x,x))) (true);
