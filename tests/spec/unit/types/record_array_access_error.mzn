/***
!Test
solvers: [gecode]
expected:
- !Error
  type: TypeError
type: compile
***/

array[1..2] of record(array[1..2] of var int: arrField): x;
var 1..2: i;
constraint x[i]; % variable array access not allowed when a record contains an array