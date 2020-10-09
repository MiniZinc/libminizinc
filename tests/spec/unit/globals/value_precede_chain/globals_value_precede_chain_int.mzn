/***
!Test
solvers: [gecode, chuffed]
expected: !Result
  solution: !SolutionSet
  - !Solution
    x: [1, 1, 1]
  - !Solution
    x: [4, 1, 1]
  - !Solution
    x: [4, 1, 3]
  - !Solution
    x: [1, 1, 4]
  - !Solution
    x: [4, 1, 4]
  - !Solution
    x: [4, 3, 1]
  - !Solution
    x: [4, 3, 2]
  - !Solution
    x: [4, 3, 3]
  - !Solution
    x: [4, 3, 4]
  - !Solution
    x: [1, 4, 1]
  - !Solution
    x: [4, 4, 1]
  - !Solution
    x: [1, 4, 3]
  - !Solution
    x: [4, 4, 3]
  - !Solution
    x: [1, 4, 4]
  - !Solution
    x: [4, 4, 4]
options:
  all_solutions: true
***/

% A test for value_precede_chain for integer variables.

include "value_precede_chain.mzn";

array[1..3] of var 1..4: x ::add_to_output;

constraint value_precede_chain([3, 2, 1], [3, 2, 1]);
constraint value_precede_chain([4, 3, 2], x);
solve satisfy;
output ["x = array1d(1..3, ", show(x), ");\n"];
