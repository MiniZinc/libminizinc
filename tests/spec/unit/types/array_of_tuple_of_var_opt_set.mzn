/***
!Test
solvers: [gecode, chuffed]
expected: !Result
  solution: !Solution
    v: 1
    x: [[!!set {1}], [null]]
***/

var 1..1: v;
any: x :: output = [({v},), (<>,)];
