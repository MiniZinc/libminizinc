/***
!Test
check_against: []
expected:
- !Result
  solution: !Solution
    x: [2, 1]
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

array[1..2] of var 1..3: x;
array[1..2] of var bool: b = [ x[i] = 1  | i in 1..2 ];
array[1..2] of var bool: c = [ x[1] = 1, x[2] = 1 ];

constraint x[1] = 2;
