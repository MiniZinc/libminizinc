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

var 1..1: x;
var 1..1: y = sum(i in 1..x)(let { var 1..1: z; } in 1);
