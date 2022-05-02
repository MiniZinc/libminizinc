/***
--- !Test
solvers: [gecode]
options:
  all_solutions: true
  -D: x = -2..2; y = 0
expected: !Result
  solution: !SolutionSet
  - !Solution
    b: -2
    z: 1
  - !Solution
    b: -1
    z: 1
  - !Solution
    b: 0
    z: 1
  - !Solution
    b: 1
    z: 1
  - !Solution
    b: 2
    z: 1
--- !Test
solvers: [gecode]
options:
  all_solutions: true
  -D: x = -2..2; y = 1
expected: !Result
  solution: !SolutionSet
  - !Solution
    b: -2
    z: -2
  - !Solution
    b: -1
    z: -1
  - !Solution
    b: 0
    z: 0
  - !Solution
    b: 1
    z: 1
  - !Solution
    b: 2
    z: 2
--- !Test
solvers: [gecode]
options:
  all_solutions: true
  -D: x = 0..1; y = 2
expected: !Result
  solution: !SolutionSet
  - !Solution
    b: 0
    z: 0
  - !Solution
    b: 1
    z: 1
--- !Test
solvers: [gecode]
options:
  all_solutions: true
  -D: x = -1..1; y = 3
expected: !Result
  solution: !SolutionSet
  - !Solution
    b: -1
    z: -1
  - !Solution
    b: 0
    z: 0
  - !Solution
    b: 1
    z: 1
--- !Test
solvers: [gecode]
options:
  all_solutions: true
  -D: x = 1..3; y = -1
expected: !Result
  solution: !SolutionSet
  - !Solution
    b: 1
    z: 1
  - !Solution
    b: 2
    z: 0
  - !Solution
    b: 3
    z: 0
--- !Test
solvers: [gecode]
options:
  all_solutions: true
  -D: x = 2..3; y = 2
expected: !Result
  solution: !SolutionSet
  - !Solution
    b: 2
    z: 4
  - !Solution
    b: 3
    z: 9
--- !Test
solvers: [gecode]
options:
  all_solutions: true
  -D: x = -1..2; y = -2
expected: !Result
  solution: !SolutionSet
  - !Solution
    b: -1
    z: 1
  - !Solution
    b: 1
    z: 1
  - !Solution
    b: 2
    z: 0
***/

% Test for pow with var base, par exponent

set of int: x;
var x: b;
int: y;
var int: z;

constraint z = pow(b, y);
