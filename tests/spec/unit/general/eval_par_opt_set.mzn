/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    x: null
***/
opt set of int: x :: output = if false then {1} else <> endif;
