/***
!Test
solvers: [gecode]
options:
  all_solutions: true
expected: !Result
  solution: !SolutionSet
  - !Solution
    x: {"g": {"x": 1 }}
  - !Solution
    x: {"g": {"x": 2 }}
  - !Solution
    x: {"g": {"x": 3 }}
***/

set of int: s = 1..max([1,2,3]);
type T = record(var s: x);
type R = record(var T: g);
R: x;