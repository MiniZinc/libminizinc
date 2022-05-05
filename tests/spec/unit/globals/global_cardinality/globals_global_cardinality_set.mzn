/***
!Test
solvers: [gecode, chuffed]
options:
  all_solutions: true
expected: !Result
  solution: !SolutionSet
  - !Solution
    x: [!!set {}, !!set {}]
    counts: [0, 0]
    empty: []
    zero: [0, 0, 0]
  - !Solution
    x: [!!set {}, !!set {1}]
    counts: [1, 0]
    empty: []
    zero: [0, 0, 0]
  - !Solution
    x: [!!set {1}, !!set {}]
    counts: [1, 0]
    empty: []
    zero: [0, 0, 0]
  - !Solution
    x: [!!set {1}, !!set {1}]
    counts: [2, 0]
    empty: []
    zero: [0, 0, 0]
  - !Solution
    x: [!!set {}, !!set {2}]
    counts: [0, 1]
    empty: []
    zero: [0, 0, 0]
  - !Solution
    x: [!!set {2}, !!set {}]
    counts: [0, 1]
    empty: []
    zero: [0, 0, 0]
  - !Solution
    x: [!!set {}, !Range 1..2]
    counts: [1, 1]
    empty: []
    zero: [0, 0, 0]
  - !Solution
    x: [!!set {2}, !!set {1}]
    counts: [1, 1]
    empty: []
    zero: [0, 0, 0]
  - !Solution
    x: [!!set {1}, !!set {2}]
    counts: [1, 1]
    empty: []
    zero: [0, 0, 0]
  - !Solution
    x: [!Range 1..2, !!set {}]
    counts: [1, 1]
    empty: []
    zero: [0, 0, 0]
  - !Solution
    x: [!!set {1}, !Range 1..2]
    counts: [2, 1]
    empty: []
    zero: [0, 0, 0]
  - !Solution
    x: [!Range 1..2, !!set {1}]
    counts: [2, 1]
    empty: []
    zero: [0, 0, 0]
  - !Solution
    x: [!!set {2}, !!set {2}]
    counts: [0, 2]
    empty: []
    zero: [0, 0, 0]
  - !Solution
    x: [!!set {2}, !Range 1..2]
    counts: [1, 2]
    empty: []
    zero: [0, 0, 0]
  - !Solution
    x: [!Range 1..2, !!set {2}]
    counts: [1, 2]
    empty: []
    zero: [0, 0, 0]
  - !Solution
    x: [!Range 1..2, !Range 1..2]
    counts: [2, 2]
    empty: []
    zero: [0, 0, 0]
***/

include "global_cardinality.mzn";

constraint global_cardinality([{1}, 0..2, 1..4], [1, 2, 3], [3, 2, 1]);

array [1..2] of var set of 1..2: x;
array [1..2] of var -10..10: counts;
constraint global_cardinality(x, [1, 2], counts);

array [1..0] of var set of 0..4: empty;
array [1..3] of var -10..10: zero;
constraint global_cardinality(empty, [1, 2, 3], zero);

solve satisfy;
