/***
!Test
solvers: [gecode, chuffed]
options:
  all_solutions: true
expected: !Result
  solution: !SolutionSet
  - !Solution
    x: [1, null, 2, null, 3]
    y: 1
  - !Solution
    x: [1, null, 3, null, 2]
    y: 1
  - !Solution
    x: [2, null, 1, null, 3]
    y: 3
  - !Solution
    x: [3, null, 1, null, 2]
    y: 3
  - !Solution
    x: [2, null, 3, null, 1]
    y: 5
  - !Solution
    x: [3, null, 2, null, 1]
    y: 5
***/

include "globals.mzn";

array [1..5] of var opt 1..3: x;
var 1..5: y;

constraint occurs(x[1]);
constraint forall (i in 1..4) (occurs(x[i]) = absent(x[i + 1]));
constraint y = arg_min(x);
constraint all_different(x);

solve satisfy;
