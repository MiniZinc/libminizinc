/***
!Test
solvers: [gecode]
expected:
- !Error
  type: TypeError
type: compile
***/

constraint (albus: true, audrey: false, albus: false).albus; % duplicate field albus