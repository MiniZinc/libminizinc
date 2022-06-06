/***
!Test
solvers: [gecode]
expected:
- !Error
  type: TypeError
type: compile
***/

array[1..2] of tuple(array[1..2] of var int): x;
var 1..2: i;
constraint x[i]; % variable array access not allowed when a tuple contains an array