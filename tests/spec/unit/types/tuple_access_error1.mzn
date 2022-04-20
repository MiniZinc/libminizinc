/***
!Test
solvers: [gecode]
expected:
- !Error
  type: TypeError
type: compile
***/

tuple(bool, int): double = (false, -1);
constraint double.0; % 0 is too low, access starts from 1