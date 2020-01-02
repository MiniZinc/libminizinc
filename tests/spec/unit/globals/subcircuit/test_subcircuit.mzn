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
    x: [2, 1, 4, 4]
  - !Solution
    x: [2, 3, 4, 1]
  - !Solution
    x: [2, 1, 4, 1]
  - !Solution
    x: [3, 2, 4, 1]
  - !Solution
    x: [3, 1, 4, 2]
  - !Solution
    x: [3, 1, 4, 1]
  - !Solution
    x: [1, 2, 4, 3]
  - !Solution
    x: [1, 3, 4, 2]
  - !Solution
    x: [1, 3, 2, 4]
  - !Solution
    x: [1, 3, 2, 2]
  - !Solution
    x: [1, 4, 3, 2]
  - !Solution
    x: [1, 4, 2, 2]
  - !Solution
    x: [1, 4, 1, 2]
  - !Solution
    x: [1, 4, 2, 3]
  - !Solution
    x: [1, 3, 2, 1]
  - !Solution
    x: [2, 1, 3, 4]
  - !Solution
    x: [2, 1, 3, 3]
  - !Solution
    x: [2, 3, 1, 4]
  - !Solution
    x: [2, 1, 1, 4]
  - !Solution
    x: [3, 1, 2, 4]
  - !Solution
    x: [4, 1, 2, 3]
  - !Solution
    x: [3, 4, 1, 4]
  - !Solution
    x: [3, 2, 1, 4]
  - !Solution
    x: [3, 1, 1, 4]
  - !Solution
    x: [2, 4, 1, 3]
  - !Solution
    x: [4, 2, 1, 3]
  - !Solution
    x: [4, 1, 1, 3]
  - !Solution
    x: [2, 1, 1, 3]
  - !Solution
    x: [3, 4, 2, 1]
  - !Solution
    x: [4, 3, 3, 1]
  - !Solution
    x: [2, 4, 3, 1]
  - !Solution
    x: [4, 3, 1, 1]
  - !Solution
    x: [4, 3, 1, 2]
  - !Solution
    x: [3, 4, 1, 1]
  - !Solution
    x: [2, 3, 1, 1]
  - !Solution
    x: [2, 4, 1, 1]
  - !Solution
    x: [4, 2, 3, 1]
  - !Solution
    x: [4, 1, 3, 1]
  - !Solution
    x: [4, 2, 2, 1]
  - !Solution
    x: [4, 1, 2, 1]
  - !Solution
    x: [4, 1, 3, 2]
  - !Solution
    x: [2, 1, 3, 1]
  - !Solution
    x: [3, 1, 2, 1]
  - !Solution
    x: [4, 1, 1, 2]
  - !Solution
    x: [4, 2, 1, 1]
  - !Solution
    x: [4, 1, 1, 1]
  - !Solution
    x: [3, 2, 1, 2]
  - !Solution
    x: [3, 2, 1, 1]
  - !Solution
    x: [3, 1, 1, 2]
  - !Solution
    x: [3, 1, 1, 1]
  - !Solution
    x: [2, 1, 1, 1]
--- !Test
solvers: [cbc]
expected:
- !Result
  solution: !Solution
    x: [1, 2, 3, 4]
- !Result
  solution: !Solution
    x: [2, 1, 4, 4]
- !Result
  solution: !Solution
    x: [2, 3, 4, 1]
- !Result
  solution: !Solution
    x: [2, 1, 4, 1]
- !Result
  solution: !Solution
    x: [3, 2, 4, 1]
- !Result
  solution: !Solution
    x: [3, 1, 4, 2]
- !Result
  solution: !Solution
    x: [3, 1, 4, 1]
- !Result
  solution: !Solution
    x: [1, 2, 4, 3]
- !Result
  solution: !Solution
    x: [1, 3, 4, 2]
- !Result
  solution: !Solution
    x: [1, 3, 2, 4]
- !Result
  solution: !Solution
    x: [1, 3, 2, 2]
- !Result
  solution: !Solution
    x: [1, 4, 3, 2]
- !Result
  solution: !Solution
    x: [1, 4, 2, 2]
- !Result
  solution: !Solution
    x: [1, 4, 1, 2]
- !Result
  solution: !Solution
    x: [1, 4, 2, 3]
- !Result
  solution: !Solution
    x: [1, 3, 2, 1]
- !Result
  solution: !Solution
    x: [2, 1, 3, 4]
- !Result
  solution: !Solution
    x: [2, 1, 3, 3]
- !Result
  solution: !Solution
    x: [2, 3, 1, 4]
- !Result
  solution: !Solution
    x: [2, 1, 1, 4]
- !Result
  solution: !Solution
    x: [3, 1, 2, 4]
- !Result
  solution: !Solution
    x: [4, 1, 2, 3]
- !Result
  solution: !Solution
    x: [3, 4, 1, 4]
- !Result
  solution: !Solution
    x: [3, 2, 1, 4]
- !Result
  solution: !Solution
    x: [3, 1, 1, 4]
- !Result
  solution: !Solution
    x: [2, 4, 1, 3]
- !Result
  solution: !Solution
    x: [4, 2, 1, 3]
- !Result
  solution: !Solution
    x: [4, 1, 1, 3]
- !Result
  solution: !Solution
    x: [2, 1, 1, 3]
- !Result
  solution: !Solution
    x: [3, 4, 2, 1]
- !Result
  solution: !Solution
    x: [4, 3, 3, 1]
- !Result
  solution: !Solution
    x: [2, 4, 3, 1]
- !Result
  solution: !Solution
    x: [4, 3, 1, 1]
- !Result
  solution: !Solution
    x: [4, 3, 1, 2]
- !Result
  solution: !Solution
    x: [3, 4, 1, 1]
- !Result
  solution: !Solution
    x: [2, 3, 1, 1]
- !Result
  solution: !Solution
    x: [2, 4, 1, 1]
- !Result
  solution: !Solution
    x: [4, 2, 3, 1]
- !Result
  solution: !Solution
    x: [4, 1, 3, 1]
- !Result
  solution: !Solution
    x: [4, 2, 2, 1]
- !Result
  solution: !Solution
    x: [4, 1, 2, 1]
- !Result
  solution: !Solution
    x: [4, 1, 3, 2]
- !Result
  solution: !Solution
    x: [2, 1, 3, 1]
- !Result
  solution: !Solution
    x: [3, 1, 2, 1]
- !Result
  solution: !Solution
    x: [4, 1, 1, 2]
- !Result
  solution: !Solution
    x: [4, 2, 1, 1]
- !Result
  solution: !Solution
    x: [4, 1, 1, 1]
- !Result
  solution: !Solution
    x: [3, 2, 1, 2]
- !Result
  solution: !Solution
    x: [3, 2, 1, 1]
- !Result
  solution: !Solution
    x: [3, 1, 1, 2]
- !Result
  solution: !Solution
    x: [3, 1, 1, 1]
- !Result
  solution: !Solution
    x: [2, 1, 1, 1]
***/

include "subcircuit.mzn";

array[1..4] of var 1..4: x :: add_to_output;

constraint subcircuit(x);

solve satisfy;
