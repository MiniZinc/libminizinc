/***
!Test
solvers: [gecode, chuffed]
expected: !Result
  solution: !SolutionSet
  - !Solution
    x: [null, null, null]
  - !Solution
    x: [null, null, 1]
  - !Solution
    x: [null, null, 4]
  - !Solution
    x: [1, null, null]
  - !Solution
    x: [4, null, null]
  - !Solution
    x: [1, null, 1]
  - !Solution
    x: [1, null, 4]
  - !Solution
    x: [4, null, 1]
  - !Solution
    x: [4, null, 3]
  - !Solution
    x: [4, null, 4]
  - !Solution
    x: [null, 1, null]
  - !Solution
    x: [null, 4, null]
  - !Solution
    x: [1, 1, null]
  - !Solution
    x: [1, 4, null]
  - !Solution
    x: [4, 1, null]
  - !Solution
    x: [4, 3, null]
  - !Solution
    x: [4, 4, null]
  - !Solution
    x: [null, 1, 1]
  - !Solution
    x: [null, 1, 4]
  - !Solution
    x: [null, 4, 1]
  - !Solution
    x: [null, 4, 3]
  - !Solution
    x: [null, 4, 4]
options:
  all_solutions: true
***/

% A test for value_precede_chain for integer variables.

include "value_precede_chain.mzn";

array[1..3] of var opt 1..4: x;

constraint value_precede_chain([3, 2, 1], [3, <>, 2, 1, <>]);

constraint value_precede_chain([4, 3, 2], x);
constraint exists (xi in x) (absent(xi));

solve satisfy;
