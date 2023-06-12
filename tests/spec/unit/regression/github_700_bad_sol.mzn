/***
!Test
solvers: [gecode, chuffed]
options:
  all_solutions: true
expected: !Result
  status: ALL_SOLUTIONS
  solution: !SolutionSet
  - !Solution
    c: !!set {1}
    d: !!set {}
  - !Solution
    c: !!set {}
    d: !!set {1}
***/
var set of 1..1: c;
var set of 1..1: d;
constraint c symdiff d = {1};
