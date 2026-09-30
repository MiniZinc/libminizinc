/***
!Test
solvers: [gecode]
expected: !Error
  type: TypeError
***/

% Optional fields can only be omitted in data files, not in the model
record(opt int: x, int: y): r;
r = (y: 1);
