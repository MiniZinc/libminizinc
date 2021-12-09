/***
!Test
check_against: []
expected:
- !Result
  solution: !Solution
    _output_item: 'ok'
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

include "inverse.mzn";
enum X = {a,b,c};
array[1..1,X] of var X: x;
constraint inverse(x[1,..], x[1,..]);
output ["ok"];
