/***
!Test
solvers: [gecode, cp-sat, chuffed]
options:
  all_solutions: true
expected: !Result
  solution: !SolutionSet
  - !Solution
    a: [!!set {}, null]
    i: 2
  - !Solution
    a: [!!set {1}, null]
    i: 2
  - !Solution
    a: [!!set {2}, null]
    i: 2
  - !Solution
    a: [!!set {1, 2}, null]
    i: 2
  status: ALL_SOLUTIONS
***/

% Array access with a variable index into an array of var opt sets
array [1..2] of var opt set of 1..2: a;
var 1..2: i;
constraint a[i] = <>;
constraint occurs(a[1]);
constraint (a[2] default {2}) = {2};
