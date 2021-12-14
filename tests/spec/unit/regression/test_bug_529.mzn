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

enum X = {a,b};
opt X: y::output = if true then <> else a endif;
opt X: z::output = if true then a else <> endif;
output ["ok"];
