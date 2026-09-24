/***
!Test
solvers: [gecode, chuffed]
options:
  all_solutions: true
expected: !Result
  solution: !SolutionSet
  - !Solution
    x: 1
    y: 2
    z: null
    r: null
    s: null
  - !Solution
    x: 1
    y: 2
    z: 1
    r: !!set {2}
    s: null
  - !Solution
    x: 1
    y: 2
    z: 2
    r: null
    s: !!set {2}
  - !Solution
    x: 2
    y: 1
    z: null
    r: null
    s: null
  - !Solution
    x: 2
    y: 1
    z: 1
    r: !!set {2}
    s: null
  - !Solution
    x: 2
    y: 1
    z: 2
    r: null
    s: !!set {2}
  status: ALL_SOLUTIONS
***/

% Multi-dimensional array access into arrays of var opt sets, with var and var opt indices
array [1..2, 1..2, 1..2] of var opt set of 1..2: a3;
array [1..2, 1..1, 1..1, 1..1, 1..1, 1..2] of var opt set of 1..2: a6;
constraint forall (i, j, k in 1..2) (a3[i, j, k] = if i = j then <> else {k} endif);
constraint forall (i, j in 1..2) (a6[i, 1, 1, 1, 1, j] = if i = j then <> else {j} endif);
var 1..2: x;
var 1..2: y;
var opt 1..2: z;
constraint a3[x, y, 2] = {2};
constraint a6[x, 1, 1, 1, 1, y] != <>;
var opt set of int: r :: output = a6[z, 1, 1, 1, 1, 2];
var opt set of int: s :: output = a3[z, 1, z];
