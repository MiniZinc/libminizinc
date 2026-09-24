/***
!Test
solvers: [gecode, chuffed]
options:
  all_solutions: true
expected: !Result
  solution: !SolutionSet
  - !Solution
    b: false
    x_roff: null
  - !Solution
    b: true
    x_roff: !!set {1}
  status: ALL_SOLUTIONS
***/
var bool: b;
any: x_roff :: output = if b then {1} else <> endif;
