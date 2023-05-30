/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    c: !!set {1}
    d: !Range 1..2
***/
var set of 1..1: c;
var set of 1..2: d;

constraint card(c symdiff d) = 1;
constraint card(c) > 0;
constraint card(d) > 0;
