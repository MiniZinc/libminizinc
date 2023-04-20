/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    l1: 0
***/

include "nvalue_fn.mzn";

array[1..10] of var 0..10: X;
int: l1 :: output = lb(nvalue(X));
