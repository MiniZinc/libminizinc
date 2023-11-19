/***
!Test
expected: !Result
  status: ALL_SOLUTIONS
  solution: !SolutionSet
  - !Solution
    x: [true, false, false]
    y: true
options:
  all_solutions: true
solvers: [gecode]
***/
include "member.mzn";
array [1..3] of var bool: x;
var bool: y;

constraint member(x, y);
constraint x = array1d(1..3, [true, false, false]);
constraint y = true;
