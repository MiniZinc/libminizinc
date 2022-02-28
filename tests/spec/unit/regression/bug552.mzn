/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    x: [2.0]
***/

% 2.6.0 introduced a clashing definition of arrayXd which was used intermittently
array [1..1] of float: x :: output = int2float([2]);
