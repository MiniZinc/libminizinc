/***
!Test
solvers: [gecode, cp-sat, chuffed]
options:
  all_solutions: true
expected: !Result
  solution: !SolutionSet
  - !Solution
    i: 1
    j: null
    r: !!set {1}
    q: null
  - !Solution
    i: 2
    j: null
    r: null
    q: null
  - !Solution
    i: 3
    j: null
    r: !!set {2, 3}
    q: null
  - !Solution
    i: 3
    j: 1
    r: !!set {2, 3}
    q: !!set {1}
  - !Solution
    i: 3
    j: 2
    r: !!set {2, 3}
    q: !!set {2}
  - !Solution
    i: 3
    j: 3
    r: !!set {2, 3}
    q: !!set {3}
  status: ALL_SOLUTIONS
***/

% Variable index into a par array of opt sets, and an optional index into an array of var sets
array [1..3] of opt set of int: p = [{1}, <>, {2, 3}];
array [1..3] of set of int: s = [{1}, {2}, {3}];
var 1..3: i;
var opt 1..3: j;
var opt set of int: r :: output = p[i];
var opt set of int: q :: output = s[j];
constraint occurs(q) -> i = 3;
