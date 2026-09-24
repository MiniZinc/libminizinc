/***
!Test
solvers: [gecode, cp-sat]
options:
  all_solutions: true
expected: !Result
  solution: !SolutionSet
  - !Solution
    z: !!set {1, 2}
    y: null
    it_zy: !!set {1, 2}
    it_yz: !!set {1, 2}
    df_zy: !!set {1, 2}
    df_yz: null
    mn: null
    mx: null
  - !Solution
    z: !!set {2}
    y: null
    it_zy: !!set {2}
    it_yz: !!set {2}
    df_zy: !!set {2}
    df_yz: null
    mn: null
    mx: null
  - !Solution
    z: !!set {1, 2}
    y: !!set {2}
    it_zy: !!set {2}
    it_yz: !!set {2}
    df_zy: !!set {1}
    df_yz: !!set {}
    mn: 2
    mx: 2
  - !Solution
    z: !!set {2}
    y: !!set {2}
    it_zy: !!set {2}
    it_yz: !!set {2}
    df_zy: !!set {}
    df_yz: !!set {}
    mn: 2
    mx: 2
  status: ALL_SOLUTIONS
***/

% Operations with one present and one optional operand, and min/max of var opt sets
% (min/max of the empty set is undefined, so y = {} is excluded)
var set of 1..2: z;
var opt set of {2}: y;
var set of int: it_zy :: output = z intersect y;
var set of int: it_yz :: output = y intersect z;
var set of int: df_zy :: output = z diff y;
var opt set of int: df_yz :: output = y diff z;
var opt int: mn :: output = min(y);
var opt int: mx :: output = max(y);
constraint z != {} /\ z != {1};
