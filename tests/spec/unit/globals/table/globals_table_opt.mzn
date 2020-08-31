/***
!Test
solvers: [gecode, chuffed]
expected: !Result
  status: ALL_SOLUTIONS
  solution: !SolutionSet
  - !Solution
    x: [3, 5]
  - !Solution
    x: [4, 5]
  - !Solution
    x: [5, 5]
  - !Solution
    x: [3, <>]
  - !Solution
    x: [4, <>]
  - !Solution
    x: [5, <>]
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
