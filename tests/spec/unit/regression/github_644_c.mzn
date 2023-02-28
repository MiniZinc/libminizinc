/***
!Test
check_against: []
expected:
- !Result
  solution: !Solution
    arr:
    - 1
    x: 1
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

var opt 1..1: x;
array[1..1] of var 1..1: arr;

constraint x = count(i in 1..1)(arr[i] = 1);
