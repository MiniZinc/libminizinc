/***
!Test
solvers: [gecode]
expected:
- !Error
  type: TypeError
type: compile
***/

array[1..2] of tuple(array[1..2] of var int): x;
var bool: b;
constraint (if b then x[1] else x[2] endif).1[1] < 2; % variable if-then-else not allowed with tuple types that contain an array