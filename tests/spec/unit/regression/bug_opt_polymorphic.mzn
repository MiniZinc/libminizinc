/***
!Test
check_against: []
expected:
- !Result
  solution: !Solution
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

predicate f(array[int] of opt $E, var int: x) = x=1;
enum E = { A };
var 1..3: x;
constraint f([<>, A], x);
