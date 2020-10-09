/***
!Test
expected:
- !Result
  solution: !SolutionSet
  - !Solution
    x:
    - null
    - null
    - null
  - !Solution
    x:
    - true
    - null
    - null
  - !Solution
    x:
    - null
    - true
    - null
  - !Solution
    x:
    - true
    - true
    - null
  - !Solution
    x:
    - null
    - null
    - true
  - !Solution
    x:
    - true
    - null
    - true
  - !Solution
    x:
    - null
    - true
    - true
  - !Solution
    x: [true, true, true]
  status: ALL_SOLUTIONS
options:
  all_solutions: true
solvers: [gecode, chuffed]
***/

array [1..3] of var opt bool: x;

constraint forall (x);