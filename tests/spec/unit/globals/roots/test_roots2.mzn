/***
!Test
solvers: [gecode, chuffed]
expected: !Result
  solution: !SolutionSet
  - !Solution
    s: !Range 1..5
    t: !Range 1..5
  - !Solution
    s: !Range 1..5
    t: !Range 1..4
  - !Solution
    s: !Range 1..5
    t: !!set {1, 2, 3, 5}
  - !Solution
    s: !Range 1..5
    t: !Range 1..3
  - !Solution
    s: !!set {1, 2, 3, 5}
    t: !!set {1, 3, 4, 5}
  - !Solution
    s: !!set {1, 2, 3, 5}
    t: !!set {1, 3, 4}
  - !Solution
    s: !!set {1, 2, 3, 5}
    t: !!set {1, 3, 5}
  - !Solution
    s: !!set {1, 2, 3, 5}
    t: !!set {1, 3}
  - !Solution
    s: !!set {1, 3, 4}
    t: !!set {1, 2, 4, 5}
  - !Solution
    s: !!set {1, 3, 4}
    t: !!set {1, 2, 4}
  - !Solution
    s: !!set {1, 3, 4}
    t: !!set {1, 2, 5}
  - !Solution
    s: !!set {1, 3, 4}
    t: !Range 1..2
  - !Solution
    s: !!set {1, 3}
    t: !!set {1, 4, 5}
  - !Solution
    s: !!set {1, 3}
    t: !!set {1, 4}
  - !Solution
    s: !!set {1, 3}
    t: !!set {1, 5}
  - !Solution
    s: !!set {1, 3}
    t: !!set {1}
  - !Solution
    s: !!set {2, 4, 5}
    t: !Range 2..5
  - !Solution
    s: !!set {2, 4, 5}
    t: !Range 2..4
  - !Solution
    s: !!set {2, 4, 5}
    t: !!set {2, 3, 5}
  - !Solution
    s: !!set {2, 4, 5}
    t: !Range 2..3
  - !Solution
    s: !!set {2, 5}
    t: !Range 3..5
  - !Solution
    s: !!set {2, 5}
    t: !Range 3..4
  - !Solution
    s: !!set {2, 5}
    t: !!set {3, 5}
  - !Solution
    s: !!set {2, 5}
    t: !!set {3}
  - !Solution
    s: !!set {4}
    t: !!set {2, 4, 5}
  - !Solution
    s: !!set {4}
    t: !!set {2, 4}
  - !Solution
    s: !!set {4}
    t: !!set {2, 5}
  - !Solution
    s: !!set {4}
    t: !!set {2}
  - !Solution
    s: !!set {}
    t: !Range 4..5
  - !Solution
    s: !!set {}
    t: !!set {4}
  - !Solution
    s: !!set {}
    t: !!set {5}
  - !Solution
    s: !!set {}
    t: !!set {}
options:
  all_solutions: true
***/

include "roots.mzn";

var set of 1..5: s ::add_to_output;
var set of 1..5: t ::add_to_output;

constraint roots([1, 3, 1, 2, 3], s, t);

solve satisfy;

output [
   "s = ", show(s), ";\n",
   "t = ", show(t), ";\n"
];
