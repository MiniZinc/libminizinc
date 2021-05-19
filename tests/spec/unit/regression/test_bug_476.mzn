/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    b: 1
    c: true
  status: SATISFIED
***/

var 1..9: b;
var bool: c;
constraint b = c;
solve satisfy;
