/***
!Test
solvers: [gecode, chuffed]
options:
  all_solutions: true
expected: !Result
  solution: !SolutionSet
  - !Solution
    x: [!!set {1}, !Range 1..2]
  - !Solution
    x: [!!set {}, !Range 1..2]
  - !Solution
    x: [!Range 1..2, !!set {1}]
  - !Solution
    x: [!!set {1}, !!set {1}]
  - !Solution
    x: [!!set {2}, !!set {1}]
  - !Solution
    x: [!!set {}, !!set {1}]
  - !Solution
    x: [!!set {1}, !!set {2}]
  - !Solution
    x: [!Range 1..2, !!set {}]
  - !Solution
    x: [!!set {1}, !!set {}]
***/

include "global_cardinality.mzn";

constraint global_cardinality([{1}, 0..2, 1..4], [1, 2, 3], [3, 2, 1]);

array [1..2] of var set of 1..2: x;
constraint global_cardinality(x, [1, 2], [1, 0], [2, 1]);

solve satisfy;
