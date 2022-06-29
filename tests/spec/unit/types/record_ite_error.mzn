/***
!Test
solvers: [gecode]
expected:
- !Error
  type: TypeError
type: compile
***/

array[1..2] of record(array[1..2] of var int: a): x;
var bool: b;
constraint (if b then x[1] else x[2] endif).a[1] < 2; % variable if-then-else not allowed with record types that contain an array