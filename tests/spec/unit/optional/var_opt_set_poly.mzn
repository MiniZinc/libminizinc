/***
!Test
solvers: [gecode, cp-sat, chuffed]
options:
  all_solutions: true
expected: !Result
  solution: !SolutionSet
  - !Solution
    y: !!set {1}
    w: !!set {1}
    z: !!set {1}
    a: !!set {1}
  - !Solution
    y: !!set {2}
    w: !!set {2}
    z: !!set {2}
    a: !!set {1}
  status: ALL_SOLUTIONS
***/

% var opt $T and any $T can be instantiated with var opt set of int
function var opt $T: g(var opt $T: x) = x;
function any $T: idt(any $T: x) = x;
predicate eqq(var opt $T: x, var opt $T: y) = x = y;

var set of 1..2: y;
var opt set of 1..2: w;
constraint card(y) = 1;

any: z0 = g(y);
constraint occurs(z0);
constraint z0 = w;
var opt set of int: z :: output = g(y);

opt set of int: p = {1};
var opt set of 1..2: a;
constraint eqq(p, a);
constraint idt(a) != <>;
