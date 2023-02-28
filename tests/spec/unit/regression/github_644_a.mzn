/***
!Test
check_against: []
expected:
- !Result
  solution: !Solution
    arr:
    - null
    - null
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

array[1..2] of var opt bool: arr;

constraint arr[1] = true -> arr[2] = true;
