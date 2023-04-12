/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
***/

var bool: b;
var bool: c;
constraint b = (c /\ <> /\ true);
