/***
!Test
check_against: []
expected:
- !Result
  solution: !Solution
    _output_item: WORKED
  status: SATISFIED
extra_files: []
markers: []
options:
  all_solutions: false
solvers:
- gecode
type: solve

***/

test test1(1..10: x) = true;
solve satisfy;
output [ if test1(11) then "DID NOT WORK" else "WORKED" endif ];
