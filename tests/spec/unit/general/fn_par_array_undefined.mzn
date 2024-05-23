/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    x: [2]
***/

function array [int] of int: foo(bool: b) = let {
  constraint b;
} in [1];

any: x :: output = foo(false) default [2];
