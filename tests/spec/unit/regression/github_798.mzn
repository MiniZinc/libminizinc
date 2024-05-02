/***
!Test
solvers: [gecode]
options:
  -G: gecode_presolver
expected: !Result
  status: UNSATISFIABLE
***/

% Regression test for incorrect definition of fzn_all_different_int in gecode_presolver

include "globals.mzn";
var 1..2: x;
constraint all_different([1, 1, x]);
