/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    a: false
***/

any: a :: output = let {
  tuple(1..3): x = (4,);
} in true;
