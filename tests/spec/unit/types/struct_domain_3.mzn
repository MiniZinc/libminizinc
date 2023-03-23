/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    a: false
***/

any: a :: output = let {
  tuple(array [int] of 1..1): x = ([2],);
} in true;
