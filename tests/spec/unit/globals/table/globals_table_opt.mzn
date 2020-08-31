/***
!Test
solvers: [gecode, chuffed]
expected: !Result
  status: ALL_SOLUTIONS
  solution: !SolutionSet
  - !Solution
    xs: [3, 5]
  - !Solution
    xs: [4, 5]
  - !Solution
    xs: [5, 5]
  - !Solution
    xs: [3, null]
  - !Solution
    xs: [4, null]
  - !Solution
    xs: [5, null]
options:
  all_solutions: true
***/

include "table.mzn";

array[1..2] of var opt 3..5: xs;
array[int, 1..2] of opt int: table = [|
  <>, 5,|
  5, 5,|
|];

constraint occurs(xs[1]);
constraint table(xs, table);
