/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    a: false
***/

any: a :: output = let {
  tuple(array [int] of var 1..1): x = ([2],);
} in true;
