/***
!Test
solvers: [gecode, chuffed]
options:
  all_solutions: true
expected: !Result
  solution: !SolutionSet
  - !Solution
    x: [null, null]
    y: null
  - !Solution
    x: [1, null]
    y: 1
  - !Solution
    x: [2, null]
    y: 1
  - !Solution
    x: [null, 1]
    y: 2
  - !Solution
    x: [null, 2]
    y: 2
  - !Solution
    x: [1, 2]
    y: 2
  - !Solution
    x: [2, 1]
    y: 1
***/

include "globals.mzn";

array [1..2] of var opt 1..2: x;
var opt 1..2: y :: output = arg_max_weak(x);
constraint all_different(x);

solve satisfy;
