/***
!Test
solvers: [gecode, chuffed]
options:
  all_solutions: true
expected: !Result
  solution: !SolutionSet
  - !Solution
    x: [1, 4, 1, 4]
  - !Solution
    x: [1, 4, 2, 4]
  - !Solution
    x: [1, 4, 3, 4]
***/

include "regular_regexp.mzn";

array [1..4] of var 1..4: x :: add_to_output;

constraint regular(x, "1 [^1 2 3] [1-3] 4");