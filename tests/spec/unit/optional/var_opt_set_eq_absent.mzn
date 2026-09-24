/***
!Test
solvers: [gecode, cp-sat]
options:
  all_solutions: true
expected: !Result
  solution: !SolutionSet
  - !Solution
    x: [null, !!set {}, !!set {}]
  - !Solution
    x: [null, !!set {1}, !!set {}]
  - !Solution
    x: [!!set {}, null, !!set {}]
  - !Solution
    x: [!!set {1}, null, !!set {}]
  status: ALL_SOLUTIONS
***/

% Comparisons with <> on either side, default, and bounds of var opt sets
array [1..3] of var opt set of 1..1: x;
constraint (<> = x[1]) xor (x[2] = <>);
constraint (x[3] default {1}) = {};
constraint forall (i in 1..3) (ub(x[i]) = 1..1 /\ lb(x[i]) = {});
constraint ub_array(x) = 1..1 /\ lb_array(x) = {};
