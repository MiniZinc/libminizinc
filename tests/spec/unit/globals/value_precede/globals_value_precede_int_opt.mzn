/***
!Test
solvers: [gecode, chuffed]
expected: !Result
  status: ALL_SOLUTIONS
  solution: !SolutionSet
  - !Solution
    x: [<>, <>, <>]
  - !Solution
    x: [1, <>, <>]
  - !Solution
    x: [3, <>, <>]
  - !Solution
    x: [<>, 1, <>]
  - !Solution
    x: [<>, 3, <>]
  - !Solution
    x: [1, 1, <>]
  - !Solution
    x: [1, 2, <>]
  - !Solution
    x: [1, 3, <>]
  - !Solution
    x: [3, 1, <>]
  - !Solution
    x: [3, 3, <>]
  - !Solution
    x: [<>, <>, 1]
  - !Solution
    x: [<>, <>, 3]
  - !Solution
    x: [1, <>, 1]
  - !Solution
    x: [1, <>, 2]
  - !Solution
    x: [1, <>, 3]
  - !Solution
    x: [3, <>, 1]
  - !Solution
    x: [3, <>, 3]
  - !Solution
    x: [<>, 1, 1]
  - !Solution
    x: [<>, 1, 2]
  - !Solution
    x: [<>, 1, 3]
  - !Solution
    x: [<>, 3, 1]
  - !Solution
    x: [<>, 3, 3]
  - !Solution
    x: [1, 1, 1]
  - !Solution
    x: [1, 2, 1]
  - !Solution
    x: [1, 3, 1]
  - !Solution
    x: [1, 1, 2]
  - !Solution
    x: [1, 2, 2]
  - !Solution
    x: [1, 3, 2]
  - !Solution
    x: [1, 1, 3]
  - !Solution
    x: [1, 2, 3]
  - !Solution
    x: [1, 3, 3]
  - !Solution
    x: [3, 1, 1]
  - !Solution
    x: [3, 1, 2]
  - !Solution
    x: [3, 1, 3]
  - !Solution
    x: [3, 3, 1]
  - !Solution
    x: [3, 3, 3]
options:
  all_solutions: true
***/

include "value_precede.mzn";

array[1..3] of var opt 1..3: x;

constraint fzn_value_precede_int_opt(1,2,x);
solve satisfy;
