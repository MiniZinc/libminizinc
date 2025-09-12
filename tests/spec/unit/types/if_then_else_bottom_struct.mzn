/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    x: []
***/

array [int] of tuple(int): x :: output = if true then [] else [(1,)] endif;
