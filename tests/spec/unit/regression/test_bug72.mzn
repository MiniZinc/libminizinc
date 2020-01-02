/***
--- !Test
solvers: [gecode]
options:
  all_solutions: true
expected: !Result
  status: ALL_SOLUTIONS
  solution:
  - !Solution
    x: !!set {1}
    b: true
--- !Test
solvers: [chuffed]
expected: !Result
  status: SATISFIED
  solution: !Solution
    x: !!set {1}
    b: true
***/

var set of 1..1: x;
constraint 1 in x;

var bool: b;
constraint b = exists([true | s in x]);

solve satisfy;