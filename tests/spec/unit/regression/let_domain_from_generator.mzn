/***
!Test
check_against: []
expected:
- !Result
  solution: !Solution {}
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

array[int] of var int: x = [ let { var d: x_i } in x_i | d in [ {1,3}, {4,1}] ];
