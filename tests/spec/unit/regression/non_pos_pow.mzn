/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    a: 2
  b: 8
  p: false
***/

var 2..10: a;
var 2..10: b;
var bool: p;

constraint (pow(a,3) != b) -> p; 
constraint not p;