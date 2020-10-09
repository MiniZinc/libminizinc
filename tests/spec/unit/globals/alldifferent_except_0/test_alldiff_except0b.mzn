/***
!Test
solvers: [gecode, chuffed]
expected: !Result
  solution: !SolutionSet
  - !Solution
    vs: [0, 0, 0, -1]
  - !Solution
    vs: [1, 0, 0, -1]
  - !Solution
    vs: [2, 0, 0, -1]
  - !Solution
    vs: [0, 1, 0, -1]
  - !Solution
    vs: [2, 1, 0, -1]
  - !Solution
    vs: [0, 2, 0, -1]
  - !Solution
    vs: [1, 2, 0, -1]
  - !Solution
    vs: [0, 0, 1, -1]
  - !Solution
    vs: [2, 0, 1, -1]
  - !Solution
    vs: [0, 2, 1, -1]
  - !Solution
    vs: [0, 0, 2, -1]
  - !Solution
    vs: [1, 0, 2, -1]
  - !Solution
    vs: [0, 1, 2, -1]
  - !Solution
    vs: [0, 0, -1, 0]
  - !Solution
    vs: [1, 0, -1, 0]
  - !Solution
    vs: [2, 0, -1, 0]
  - !Solution
    vs: [0, 1, -1, 0]
  - !Solution
    vs: [2, 1, -1, 0]
  - !Solution
    vs: [0, 2, -1, 0]
  - !Solution
    vs: [1, 2, -1, 0]
  - !Solution
    vs: [0, -1, 0, 0]
  - !Solution
    vs: [1, -1, 0, 0]
  - !Solution
    vs: [2, -1, 0, 0]
  - !Solution
    vs: [-1, 0, 0, 0]
  - !Solution
    vs: [0, 0, 0, 0]
  - !Solution
    vs: [1, 0, 0, 0]
  - !Solution
    vs: [2, 0, 0, 0]
  - !Solution
    vs: [-1, 1, 0, 0]
  - !Solution
    vs: [0, 1, 0, 0]
  - !Solution
    vs: [2, 1, 0, 0]
  - !Solution
    vs: [-1, 2, 0, 0]
  - !Solution
    vs: [0, 2, 0, 0]
  - !Solution
    vs: [1, 2, 0, 0]
  - !Solution
    vs: [0, -1, 1, 0]
  - !Solution
    vs: [2, -1, 1, 0]
  - !Solution
    vs: [-1, 0, 1, 0]
  - !Solution
    vs: [0, 0, 1, 0]
  - !Solution
    vs: [2, 0, 1, 0]
  - !Solution
    vs: [-1, 2, 1, 0]
  - !Solution
    vs: [0, 2, 1, 0]
  - !Solution
    vs: [0, -1, 2, 0]
  - !Solution
    vs: [1, -1, 2, 0]
  - !Solution
    vs: [-1, 0, 2, 0]
  - !Solution
    vs: [0, 0, 2, 0]
  - !Solution
    vs: [1, 0, 2, 0]
  - !Solution
    vs: [-1, 1, 2, 0]
  - !Solution
    vs: [0, 1, 2, 0]
  - !Solution
    vs: [0, 0, -1, 1]
  - !Solution
    vs: [2, 0, -1, 1]
  - !Solution
    vs: [0, 2, -1, 1]
  - !Solution
    vs: [0, -1, 0, 1]
  - !Solution
    vs: [2, -1, 0, 1]
  - !Solution
    vs: [-1, 0, 0, 1]
  - !Solution
    vs: [0, 0, 0, 1]
  - !Solution
    vs: [2, 0, 0, 1]
  - !Solution
    vs: [-1, 2, 0, 1]
  - !Solution
    vs: [0, 2, 0, 1]
  - !Solution
    vs: [0, -1, 2, 1]
  - !Solution
    vs: [-1, 0, 2, 1]
  - !Solution
    vs: [0, 0, 2, 1]
  - !Solution
    vs: [0, 0, -1, 2]
  - !Solution
    vs: [1, 0, -1, 2]
  - !Solution
    vs: [0, 1, -1, 2]
  - !Solution
    vs: [0, -1, 0, 2]
  - !Solution
    vs: [1, -1, 0, 2]
  - !Solution
    vs: [-1, 0, 0, 2]
  - !Solution
    vs: [0, 0, 0, 2]
  - !Solution
    vs: [1, 0, 0, 2]
  - !Solution
    vs: [-1, 1, 0, 2]
  - !Solution
    vs: [0, 1, 0, 2]
  - !Solution
    vs: [0, -1, 1, 2]
  - !Solution
    vs: [-1, 0, 1, 2]
  - !Solution
    vs: [0, 0, 1, 2]
options:
  all_solutions: true
***/

include "alldifferent_except_0.mzn";

array[1..4] of var -1..2: vs;
constraint alldifferent_except_0(vs);
solve satisfy;
output ["vs = array1d(1..4, ", show(vs), ");"];
