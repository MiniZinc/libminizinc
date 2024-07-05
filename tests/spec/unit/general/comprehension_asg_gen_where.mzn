/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    x: [2]
    y: [1]
***/

any: x :: output = [i | i = 2 where true];
any: y :: output = [j | i = 2, j in 1..1 where true];
