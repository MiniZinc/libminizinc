/***
!Test
solvers: [gecode]
options:
  all_solutions: true
expected: !Result
  solution: !SolutionSet
  - !Solution
    i: 1
  - !Solution
    i: 2
***/

include "nosets.mzn";

array [int] of set of int: a = [{}, {}];
var 1..2: i;
constraint not (i in a[i]);
