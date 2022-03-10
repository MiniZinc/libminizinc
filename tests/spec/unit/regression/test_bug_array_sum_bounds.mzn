/***
!Test
check_against: []
expected:
- !Result
  solution: !Solution
    C: [0, 0, 0]
    y: 0
  status: SATISFIED
extra_files: []
markers: []
name: ''
options:
  all_solutions: false
solvers: [gecode]
type: solve
***/

array[1..3] of var 0..5: C;
var int: y;
constraint y = 3 * sum(C);
constraint y <= 100;