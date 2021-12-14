/***
!Test
solvers: [gecode, chuffed]
options:
  all_solutions: true
expected: !Result
  solution: !SolutionSet
  - !Solution
    x: [1, 2]
    y: 4
  - !Solution
    x: [2, 1]
    y: 5
***/

include "globals.mzn";

array [4..5] of var 1..2: x;
var int: y;

constraint y = arg_min(x);
constraint all_different(x);

solve satisfy;
