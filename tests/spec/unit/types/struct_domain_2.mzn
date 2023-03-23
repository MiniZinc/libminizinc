/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    a: false
***/

any: a :: output = let {
  tuple(var 1..3): x = (4,);
} in true;
