/***
!Test
solvers: [gecode, chuffed]
expected: !Result
  solution: !SolutionSet
  - !Solution
    x: [[1, 3], [4, 2]]
  - !Solution
    x: [[1, 2], [4, 3]]
  - !Solution
    x: [[1, 2], [3, 4]]
options:
  all_solutions: true
***/
include "all_different.mzn";
include "var_sqr_sym.mzn";

array[1..2, 1..2] of var 1..4: x;
constraint all_different(x);
constraint var_sqr_sym(x);
