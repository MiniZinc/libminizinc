/***
!Test
solvers: [gecode, chuffed]
options:
  all_solutions: true
expected:
- !Result
  solution: !SolutionSet
  - !Solution
    x: [null, 6, 5, 3, 3]
  - !Solution
    x: [null, 5, 6, 3, 3]
  - !Solution
    x: [null, 6, 3, 5, 3]
  - !Solution
    x: [null, 3, 6, 5, 3]
  - !Solution
    x: [null, 5, 3, 6, 3]
  - !Solution
    x: [null, 3, 5, 6, 3]
  - !Solution
    x: [null, 6, 3, 3, 5]
  - !Solution
    x: [null, 3, 6, 3, 5]
  - !Solution
    x: [null, 3, 3, 6, 5]
  - !Solution
    x: [null, 5, 3, 3, 6]
  - !Solution
    x: [null, 3, 5, 3, 6]
  - !Solution
    x: [null, 3, 3, 5, 6]
***/

include "global_cardinality_closed.mzn";

array[1..5] of var opt 0..8: x;

constraint
     global_cardinality_closed(
         %[3, 3, 5, 6],
         x,
         [3, 5, 6],
         [2, 1, 1],
         [3, 2, 1]);

constraint absent(x[1]);

solve satisfy;
