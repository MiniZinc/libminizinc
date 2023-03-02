/***
!Test
check_against: []
expected:
- !Result
  solution: !Solution
    b: 1
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

enum Enum = { Foo };
array[Enum] of int: arr1 = [0];
array[int] of record(1..1: x): arr2 = [(x: 1)];
var 1..1: b;
constraint arr2[b].x = 1;
