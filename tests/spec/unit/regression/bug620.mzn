/***
!Test
solver: [gecode]
expected: !Result
  status: UNSATISFIABLE
***/

var bool: c;
var bool: d;
constraint
    not(not(true xor c) -> ((d -> c) xor false));

output [show([c,d]) ++ "\n"];