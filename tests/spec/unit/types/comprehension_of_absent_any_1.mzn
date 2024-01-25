/***
!Test
solvers: [gecode]
expected: !Error
  type: TypeError
***/

% Not allowed because TypeInst should not end up with bottom type
any: x :: output = [<> | i in 1..3];
