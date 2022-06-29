/***
!Test
solvers: [gecode]
expected:
- !Error
  type: TypeError
type: compile
***/

record(bool: myfield, int: yourfield): double = (myfield: false, yourfield: -1);
constraint double.otherfield; % otherfield does not exists in double