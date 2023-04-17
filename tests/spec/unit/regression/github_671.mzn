/***
!Test
check_against: []
expected:
- !Result
  solution: !Solution
    arr:
    - true
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

array[1..1] of var bool: arr ::output = let {
  array[1..1] of var bool: res;
  constraint res[1];
} in res;

solve satisfy;
