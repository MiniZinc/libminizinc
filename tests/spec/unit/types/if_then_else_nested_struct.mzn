/***
!Test
solvers: [gecode]
options:
  all_solutions: true
expected: !Result
  solution: !SolutionSet
  - !Solution
    b: true
    x: [[1]]
  - !Solution
    b: false
    x: [[2]]
***/

var bool: b;
any: x :: output = if b then ((1,),) else ((2,),) endif;
