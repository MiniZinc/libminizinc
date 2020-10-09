/***
!Test
solvers: [gecode, chuffed]
options:
  all_solutions: true
expected: !Result
  solution: !SolutionSet
  - !Solution
    x: [1, 2]
  - !Solution
    x: [2, 2]
***/

include "regular_regexp.mzn";

array [1..2] of var 1..4: x :: add_to_output;

constraint regular(x, "1? 2 *");