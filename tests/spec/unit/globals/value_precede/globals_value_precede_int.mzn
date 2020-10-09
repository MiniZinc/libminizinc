/***
!Test
solvers: [gecode, chuffed]
expected: !Result
  solution: !SolutionSet
  - !Solution
    x: [1, 1, 3]
  - !Solution
    x: [1, 3, 3]
  - !Solution
    x: [1, 4, 3]
  - !Solution
    x: [3, 1, 3]
  - !Solution
    x: [3, 3, 3]
  - !Solution
    x: [3, 4, 3]
  - !Solution
    x: [4, 1, 3]
  - !Solution
    x: [4, 2, 3]
  - !Solution
    x: [4, 3, 3]
  - !Solution
    x: [4, 4, 3]
options:
  all_solutions: true
***/

% A test for the integer version of value_precede/3.

include "value_precede.mzn";

array[1..3] of var 1..4: x ::add_to_output;

constraint value_precede(19, 10, [1, 3, 5, 19, 3, 10]);
constraint value_precede(4, 3, [1, 2, 1, 2, 5]);	% s and t not in x.
constraint value_precede(4, 2, x) /\ x[3] = 3;

solve satisfy;

output ["x = array1d(1..3, ", show(x), ");\n"];
