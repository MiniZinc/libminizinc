/***
!Test
solvers: [gecode, cp-sat]
options:
  all_solutions: true
expected: !Result
  solution: !SolutionSet
  - !Solution
    b: false
  status: ALL_SOLUTIONS
***/

var bool: b;
var opt set of 1..2: x = if b then {1, 3} else <> endif;
