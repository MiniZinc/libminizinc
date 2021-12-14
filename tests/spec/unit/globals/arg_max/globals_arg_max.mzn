/***
!Test
solvers: [gecode, chuffed]
options:
  all_solutions: true
expected: !Result
  solution: !SolutionSet
  - !Solution
    x: [1, 2]
    y: 5
  - !Solution
    x: [2, 1]
    y: 4
***/

include "globals.mzn";

array [4..5] of var 1..2: x;
var int: y;

constraint y = arg_max(x);
constraint all_different(x);

solve satisfy;
