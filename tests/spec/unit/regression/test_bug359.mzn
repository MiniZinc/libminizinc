/***
!Test
solvers: [gecode, chuffed]
options:
  all_solutions: true
expected: !Result
  status: ALL_SOLUTIONS
  solution: !SolutionSet
  - !Solution
    p1: 1
    p2: 1
    p3: 0
    p4: 0
  - !Solution
    p1: 1
    p2: 1
    p3: 0
    p4: 1
  - !Solution
    p1: 1
    p2: 1
    p3: 1
    p4: 1
***/

var 0..1: p1;
var 0..1: p2;
var 0..1: p3;
var 0..1: p4;

constraint (not ((p1==1 /\ p2==0) \/ (p3==1 /\ p4==0)));
constraint p1 = 1;
solve satisfy;