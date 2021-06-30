/***
!Test
check_against: []
expected:
- !Result
  solution: !Solution
    x1: 1
    x2:
    - - 0
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

var 1..2: x1;
array[1..1,1..1] of var 0..1 : x2;
constraint array2set(x2[1,..]) subset 0..1;
constraint 0 <= x2[1,x1];
