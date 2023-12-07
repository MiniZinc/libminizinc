/***
!Test
solvers: [gecode]
options:
  all_solutions: true
expected: !Result
  status: ALL_SOLUTIONS
  solution: !SolutionSet
  - !Solution
    x: [!!set {1}, !!set {2}, !!set {3}]
    b: true
  - !Solution
    x: [!!set {1, 2, 3}, !!set {1, 2, 3}, !!set {1, 2, 3}]
    b: false
***/

array [1..3] of var set of 1..3: x;

var bool: b :: output = let {
  var set of int: a = array_intersect(x);
  constraint a = {};
  constraint forall (x_i in x) (card(x_i) = 1);
  constraint forall (i in 1..2) (min(x[i]) < min(x[i + 1]));
} in true;

constraint not b -> forall (x_i in x) (card(x_i) = 3);
