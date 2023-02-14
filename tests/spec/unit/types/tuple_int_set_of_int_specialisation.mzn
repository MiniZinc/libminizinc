/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    f: true
***/

test foo(any $T: x) = true;
any: f :: output = foo((1, 1..2));
