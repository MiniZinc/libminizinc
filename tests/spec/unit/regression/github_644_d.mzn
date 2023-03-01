/***
!Test
check_against: []
expected:
- !Result
  solution: !Solution
    value: null
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

array[1..1] of 1..1: arr = [1];
opt 1..1: index = <>;
function opt int: foo(opt int: x) = let { int: y = 3 } in x;
opt 1..1: value ::output = arr[foo(index)];
