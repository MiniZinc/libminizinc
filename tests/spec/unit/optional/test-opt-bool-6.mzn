/***
!Test
expected:
- !Result
  solution: !SolutionSet
  - !Solution
    x:
    - true
    - null
    - null
  - !Solution
    x:
    - true
    - false
    - null
  - !Solution
    x:
    - true
    - null
    - false
  - !Solution
    x: [true, false, false]
  - !Solution
    x:
    - null
    - true
    - null
  - !Solution
    x:
    - false
    - true
    - null
  - !Solution
    x:
    - null
    - true
    - false
  - !Solution
    x: [false, true, false]
  - !Solution
    x:
    - true
    - true
    - null
  - !Solution
    x: [true, true, false]
  - !Solution
    x:
    - null
    - null
    - true
  - !Solution
    x:
    - false
    - null
    - true
  - !Solution
    x:
    - null
    - false
    - true
  - !Solution
    x: [false, false, true]
  - !Solution
    x:
    - true
    - null
    - true
  - !Solution
    x: [true, false, true]
  - !Solution
    x:
    - null
    - true
    - true
  - !Solution
    x: [false, true, true]
  - !Solution
    x: [true, true, true]
  status: ALL_SOLUTIONS
options:
  all_solutions: true
solvers: [gecode, chuffed]
***/

array [1..3] of var opt bool: x;

constraint exists (x);