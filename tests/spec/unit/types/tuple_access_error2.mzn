/***
!Test
solvers: [gecode]
expected:
- !Error
  type: TypeError
type: compile
***/

tuple(bool, int): double = (false, -1);
constraint double.3; % 3 is too high, double only has 2 fields