/***
!Test
solvers: [gecode, chuffed]
expected: !Result
  status: ALL_SOLUTIONS
  solution: !SolutionSet
  - !Solution
    x: [null, null, null]
  - !Solution
    x: [1, null, null]
  - !Solution
    x: [3, null, null]
  - !Solution
    x: [null, 1, null]
  - !Solution
    x: [null, 3, null]
  - !Solution
    x: [1, 1, null]
  - !Solution
    x: [1, 2, null]
  - !Solution
    x: [1, 3, null]
  - !Solution
    x: [3, 1, null]
  - !Solution
    x: [3, 3, null]
  - !Solution
    x: [null, null, 1]
  - !Solution
    x: [null, null, 3]
  - !Solution
    x: [1, null, 1]
  - !Solution
    x: [1, null, 2]
  - !Solution
    x: [1, null, 3]
  - !Solution
    x: [3, null, 1]
  - !Solution
    x: [3, null, 3]
  - !Solution
    x: [null, 1, 1]
  - !Solution
    x: [null, 1, 2]
  - !Solution
    x: [null, 1, 3]
  - !Solution
    x: [null, 3, 1]
  - !Solution
    x: [null, 3, 3]
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
