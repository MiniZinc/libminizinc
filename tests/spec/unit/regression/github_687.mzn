/***
!Test
solvers: [gecode]
expected: 
  - !Result
    solution: !Solution
      r: {x: 0}
***/
type R = record(int: x);
var R: r;

constraint r.x >= 0 /\ r.x <= 0;
