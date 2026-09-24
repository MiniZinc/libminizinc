/***
!Test
solvers: [gecode, cp-sat, chuffed]
options:
  all_solutions: true
expected: !Result
  solution: !SolutionSet
  - !Solution
    x: !!set {A, B}
    ys: [null, !!set {C}]
    d: !!set {A, B}
    xi: !!set {1, 2}
    yi: [null, !!set {3}]
  - !Solution
    x: !!set {A, C}
    ys: [null, !!set {C}]
    d: !!set {A, C}
    xi: !!set {1, 3}
    yi: [null, !!set {3}]
  status: ALL_SOLUTIONS
***/

% occurs/deopt/absent on var opt sets of enums keep the enum type (and do not warn)
enum E = {A, B, C};
var opt set of E: x;
array [1..2] of var opt set of {A, C}: ys;
constraint occurs(x) /\ card(deopt(x)) = 2;
var set of E: d :: output = deopt(x);
constraint A in deopt(x);
constraint absent(ys[1]) /\ deopt(ys)[2] = {C};
var opt set of int: xi :: output = enum2int(x);
array [int] of var opt set of int: yi :: output = enum2int(ys);
