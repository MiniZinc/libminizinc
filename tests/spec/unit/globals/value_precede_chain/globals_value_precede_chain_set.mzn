/***
!Test
solvers: [gecode, chuffed]
expected: !Result
  solution: !SolutionSet
  - !Solution
    x:
    - !Range 1..4
    - !Range 1..4
  - !Solution
    x:
    - !!set {1, 3, 4}
    - !Range 1..4
  - !Solution
    x:
    - !!set {1, 4}
    - !Range 1..4
  - !Solution
    x:
    - !!set {1}
    - !Range 1..4
  - !Solution
    x:
    - !Range 2..4
    - !Range 1..4
  - !Solution
    x:
    - !Range 3..4
    - !Range 1..4
  - !Solution
    x:
    - !!set {4}
    - !Range 1..4
  - !Solution
    x:
    - !!set {}
    - !Range 1..4
  - !Solution
    x:
    - !!set {1, 4}
    - !Range 1..3
  - !Solution
    x:
    - !!set {4}
    - !Range 1..3
  - !Solution
    x:
    - !!set {1, 3, 4}
    - !!set {1, 2, 4}
  - !Solution
    x:
    - !Range 3..4
    - !!set {1, 2, 4}
  - !Solution
    x:
    - !!set {1, 3, 4}
    - !Range 1..2
  - !Solution
    x:
    - !Range 3..4
    - !Range 1..2
  - !Solution
    x:
    - !Range 1..4
    - !!set {1, 3, 4}
  - !Solution
    x:
    - !!set {1, 3, 4}
    - !!set {1, 3, 4}
  - !Solution
    x:
    - !!set {1, 4}
    - !!set {1, 3, 4}
  - !Solution
    x:
    - !!set {1}
    - !!set {1, 3, 4}
  - !Solution
    x:
    - !Range 2..4
    - !!set {1, 3, 4}
  - !Solution
    x:
    - !Range 3..4
    - !!set {1, 3, 4}
  - !Solution
    x:
    - !!set {4}
    - !!set {1, 3, 4}
  - !Solution
    x:
    - !!set {}
    - !!set {1, 3, 4}
  - !Solution
    x:
    - !!set {1, 4}
    - !!set {1, 3}
  - !Solution
    x:
    - !!set {4}
    - !!set {1, 3}
  - !Solution
    x:
    - !Range 1..4
    - !!set {1, 4}
  - !Solution
    x:
    - !!set {1, 3, 4}
    - !!set {1, 4}
  - !Solution
    x:
    - !!set {1, 4}
    - !!set {1, 4}
  - !Solution
    x:
    - !!set {1}
    - !!set {1, 4}
  - !Solution
    x:
    - !Range 2..4
    - !!set {1, 4}
  - !Solution
    x:
    - !Range 3..4
    - !!set {1, 4}
  - !Solution
    x:
    - !!set {4}
    - !!set {1, 4}
  - !Solution
    x:
    - !!set {}
    - !!set {1, 4}
  - !Solution
    x:
    - !Range 1..4
    - !!set {1}
  - !Solution
    x:
    - !!set {1, 3, 4}
    - !!set {1}
  - !Solution
    x:
    - !!set {1, 4}
    - !!set {1}
  - !Solution
    x:
    - !!set {1}
    - !!set {1}
  - !Solution
    x:
    - !Range 2..4
    - !!set {1}
  - !Solution
    x:
    - !Range 3..4
    - !!set {1}
  - !Solution
    x:
    - !!set {4}
    - !!set {1}
  - !Solution
    x:
    - !!set {}
    - !!set {1}
  - !Solution
    x:
    - !Range 1..4
    - !Range 2..4
  - !Solution
    x:
    - !!set {1, 3, 4}
    - !Range 2..4
  - !Solution
    x:
    - !!set {1, 4}
    - !Range 2..4
  - !Solution
    x:
    - !!set {1}
    - !Range 2..4
  - !Solution
    x:
    - !Range 2..4
    - !Range 2..4
  - !Solution
    x:
    - !Range 3..4
    - !Range 2..4
  - !Solution
    x:
    - !!set {4}
    - !Range 2..4
  - !Solution
    x:
    - !!set {}
    - !Range 2..4
  - !Solution
    x:
    - !!set {1, 4}
    - !Range 2..3
  - !Solution
    x:
    - !!set {4}
    - !Range 2..3
  - !Solution
    x:
    - !!set {1, 3, 4}
    - !!set {2, 4}
  - !Solution
    x:
    - !Range 3..4
    - !!set {2, 4}
  - !Solution
    x:
    - !!set {1, 3, 4}
    - !!set {2}
  - !Solution
    x:
    - !Range 3..4
    - !!set {2}
  - !Solution
    x:
    - !Range 1..4
    - !Range 3..4
  - !Solution
    x:
    - !!set {1, 3, 4}
    - !Range 3..4
  - !Solution
    x:
    - !!set {1, 4}
    - !Range 3..4
  - !Solution
    x:
    - !!set {1}
    - !Range 3..4
  - !Solution
    x:
    - !Range 2..4
    - !Range 3..4
  - !Solution
    x:
    - !Range 3..4
    - !Range 3..4
  - !Solution
    x:
    - !!set {4}
    - !Range 3..4
  - !Solution
    x:
    - !!set {}
    - !Range 3..4
  - !Solution
    x:
    - !!set {1, 4}
    - !!set {3}
  - !Solution
    x:
    - !!set {4}
    - !!set {3}
  - !Solution
    x:
    - !Range 1..4
    - !!set {4}
  - !Solution
    x:
    - !!set {1, 3, 4}
    - !!set {4}
  - !Solution
    x:
    - !!set {1, 4}
    - !!set {4}
  - !Solution
    x:
    - !!set {1}
    - !!set {4}
  - !Solution
    x:
    - !Range 2..4
    - !!set {4}
  - !Solution
    x:
    - !Range 3..4
    - !!set {4}
  - !Solution
    x:
    - !!set {4}
    - !!set {4}
  - !Solution
    x:
    - !!set {}
    - !!set {4}
  - !Solution
    x:
    - !Range 1..4
    - !!set {}
  - !Solution
    x:
    - !!set {1, 3, 4}
    - !!set {}
  - !Solution
    x:
    - !!set {1, 4}
    - !!set {}
  - !Solution
    x:
    - !!set {1}
    - !!set {}
  - !Solution
    x:
    - !Range 2..4
    - !!set {}
  - !Solution
    x:
    - !Range 3..4
    - !!set {}
  - !Solution
    x:
    - !!set {4}
    - !!set {}
  - !Solution
    x:
    - !!set {}
    - !!set {}
options:
  all_solutions: true
***/

include "value_precede_chain.mzn";

% A test for value_precede_chain for set variables.

array[1..2] of var set of 1..4: x;

constraint value_precede_chain([3, 2, 1], [{3}, {2}, {1}]);
constraint value_precede_chain([4, 3, 2], x);
solve satisfy;
output ["x = array1d(1..2, ", show(x), ");\n"];
