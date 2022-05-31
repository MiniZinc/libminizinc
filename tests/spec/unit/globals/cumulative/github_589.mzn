/***
!Test
options:
  all_solutions: true
  -G: std
solvers: [gecode]
expected: !Result
  solution: !SolutionSet
  status: ALL_SOLUTIONS
  solution: !SolutionSet
  - !Solution
    d: [10000, 0, 0]
  - !Solution
    d: [10000, 1, 0]
  - !Solution
    d: [10000, 0, 1]
***/

include "cumulative.mzn";

array[1..3] of int: s = [0, 0, 0];
array[1..3] of var 0..10000: d;
array[1..3] of int: r = [1, 1, 1];

constraint d[1] = 10000;
constraint d[2] in 0..1;
constraint d[3] in 0..1;
constraint cumulative(s, d, r, 2);