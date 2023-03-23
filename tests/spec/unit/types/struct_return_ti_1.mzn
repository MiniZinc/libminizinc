/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    x: false
***/

function tuple(1..2): foo() = (3,);
any: x :: output = let { any: a =  foo(); } in true;
