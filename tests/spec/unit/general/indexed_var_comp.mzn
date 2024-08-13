/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    x: [1, 2, 3]
***/

var set of 1..3: s;
any: x :: output = [i + 1: i | i in s];
solve maximize card(s);
