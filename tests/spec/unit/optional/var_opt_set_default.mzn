/***
!Test
solvers: [gecode, cp-sat, chuffed]
options:
  all_solutions: true
expected: !Result
  solution: !SolutionSet
  - !Solution
    x: null
    y: !!set {2}
    z: !!set {}
    d1: !!set {}
    d2: !!set {2}
  - !Solution
    x: null
    y: !!set {2}
    z: !!set {1}
    d1: !!set {1}
    d2: !!set {2}
  - !Solution
    x: !!set {1}
    y: null
    z: !!set {}
    d1: !!set {1}
    d2: !!set {1}
  - !Solution
    x: !!set {1}
    y: null
    z: !!set {1}
    d1: !!set {1}
    d2: !!set {1}
  - !Solution
    x: !!set {1}
    y: !!set {}
    z: !!set {}
    d1: !!set {1}
    d2: !!set {1}
  - !Solution
    x: !!set {1}
    y: !!set {}
    z: !!set {1}
    d1: !!set {1}
    d2: !!set {1}
  status: ALL_SOLUTIONS
***/

% default with var set and var opt set right-hand sides
var opt set of {1}: x;
var opt set of 1..2: y;
var set of {1}: z;
var set of int: d1 :: output = x default z;
var opt set of int: d2 :: output = x default y;
opt set of int: p = <>;
constraint (p default z) = z;
constraint x != {};
constraint absent(x) -> d2 = {2};
constraint occurs(x) -> y != {1} /\ y != 1..2 /\ y != {2};
