/***
!Test
solvers: [gecode]
options:
  all_solutions: true
  keep-paths: true
expected: !Result
  status: ALL_SOLUTIONS
  solution: !SolutionSet
  - !Solution
    x: [null, null]
  - !Solution
    x: [1, null]
  - !Solution
    x: [null, 1]
  - !Solution
    x: [1, 1]
***/

% Previously the mzn_reverse_map_var added at the end of flattening for each
% element would have the same path and would get incorrectly unified
array [1..2] of var opt 1..1: x;
