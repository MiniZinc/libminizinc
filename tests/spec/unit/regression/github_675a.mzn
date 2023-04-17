/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    t: [false]
***/
tuple(var bool): t;
constraint not t.1;
