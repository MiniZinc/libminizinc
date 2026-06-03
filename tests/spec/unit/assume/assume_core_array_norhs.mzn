/***
!Test
solvers: [chuffed]
type: core
expected: !Result
  status: UNSATISFIABLE
  core: ["asmp[1]", "asmp[2]"]
***/

% The assumptions are passed as an array variable that has no right-hand side, so the
% individual element expressions cannot be recovered. The core falls back to an indexed
% reference into the argument (``asmp[1]``, ``asmp[2]``).
include "experimental/assume.mzn";
var 1..10: x;
array [1..2] of var bool: asmp;
constraint asmp[1] = (x < 3);
constraint asmp[2] = (x > 8);
constraint assume(asmp);
solve satisfy;
