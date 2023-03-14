/***
--- !Test
solvers: [gecode, chuffed]
options:
  all_solutions: true
expected: !Result
  status: ALL_SOLUTIONS
  solution: !SolutionSet
  - !Solution
    x: [1, 2, 3, 4]
  - !Solution
    x: [2, 1, 3, 4]
  - !Solution
    x: [1, 4, 3, 2]
  - !Solution
    x: [4, 2, 3, 1]
  - !Solution
    x: [2, 4, 3, 1]
  - !Solution
    x: [4, 1, 3, 2]
  - !Solution
    x: [1, 2, 4, 3]
  - !Solution
    x: [1, 3, 2, 4]
  - !Solution
    x: [1, 3, 4, 2]
  - !Solution
    x: [1, 4, 2, 3]
  - !Solution
    x: [3, 2, 1, 4]
  - !Solution
    x: [4, 2, 1, 3]
  - !Solution
    x: [3, 2, 4, 1]
  - !Solution
    x: [2, 3, 1, 4]
  - !Solution
    x: [3, 1, 2, 4]
  - !Solution
    x: [4, 3, 1, 2]
  - !Solution
    x: [2, 3, 4, 1]
  - !Solution
    x: [3, 4, 2, 1]
  - !Solution
    x: [4, 1, 2, 3]
  - !Solution
    x: [2, 4, 1, 3]
  - !Solution
    x: [3, 1, 4, 2]
***/

include "subcircuit.mzn";

array[1..4] of var 1..4: x :: add_to_output;

constraint subcircuit(x);

solve satisfy;
