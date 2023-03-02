/***
!Test
check_against: []
expected:
- !Result
  solution: !Solution
    i: 0
    x: !Range 6..10
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

var set of 1..10: x;
var int: i ::output = count(j in x)(j <= 5);
