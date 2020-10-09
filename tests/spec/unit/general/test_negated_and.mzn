/***
!Test
solvers: [gecode, chuffed]
options:
  all_solutions: true
expected: !Result
  status: ALL_SOLUTIONS
  solution: !SolutionSet
  - !Solution
    x: false
    y: false
    z: false
  - !Solution
    x: false
    y: false
    z: true
  - !Solution
    x: false
    y: true
    z: false
  - !Solution
    x: false
    y: true
    z: true
  - !Solution
    x: true
    y: false
    z: false
  - !Solution
    x: true
    y: false
    z: true
  - !Solution
    x: true
    y: true
    z: false
***/

var bool: x;
var bool: y;
var bool: z;

constraint not(x /\ y /\ z);

solve satisfy;