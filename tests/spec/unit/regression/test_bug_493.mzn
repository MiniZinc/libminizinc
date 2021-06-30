/***
!Test
check_against: []
expected:
- !Result
  solution: !Solution
    _output_item: '{}'
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

set of int: S = { };
function var set of S: asVar() = { val | val in S };
var set of S: asVar = asVar();
output [show(asVar)];