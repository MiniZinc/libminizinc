/***
!Test
check_against: []
expected:
- !Result
  solution: !Solution
    a:
    - null
    b:
    - 1
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

array[1..1] of var opt 1..1: a;
array[1..1] of var 1..1: b;

constraint occurs(a[b[1]]) -> a[b[1]] + b[1] - 1 = 1;
