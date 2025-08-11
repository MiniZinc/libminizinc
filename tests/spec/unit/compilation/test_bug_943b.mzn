/***
!Test
solvers: [gecode]
options:
  all_solutions: true
expected: !Result
  solution: !SolutionSet
  - !Solution
    x: 0
    z: 0
  - !Solution
    x: 0
    z: 1
  - !Solution
    x: 0
    z: 2
  - !Solution
    x: 0
    z: 3
  - !Solution
    x: 1
    z: 3
  - !Solution
    x: 2
    z: 3
  - !Solution
    x: 3
    z: 3
***/

var 0..3: x;
var {0,3}: y ::no_output;
var 0..3: z;

constraint x <= y /\ y <= z;
