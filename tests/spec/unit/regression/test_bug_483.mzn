/***
!Test
check_against: []
expected:
- !FlatZinc test_bug_483.fzn
extra_files: []
markers: []
name: ''
options:
  all_solutions: false
solvers:
- gecode
type: compile
***/

array[int] of var bool: x = let {
  array[int] of var bool: y = [true,false]
} in y[min(index_set(y))..max(index_set(y))-1];
% } in y[1..2]; % same but somewhat different error message