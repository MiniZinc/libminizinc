/***
!Test
solvers: [chuffed]
type: core
expected: !Result
  status: UNSATISFIABLE
  core: ["asmp(x)[1]", "asmp(x)[2]"]
***/

% The assumptions come from a function call returning an array, so the individual element
% expressions cannot be recovered. The core falls back to an indexed reference into the
% argument expression (``asmp(x)[1]``, ``asmp(x)[2]``).
include "experimental/assume.mzn";
var 1..10: x;
function array [int] of var bool: asmp(var int: y) = [y < 3, y > 8];
constraint assume(asmp(x));
solve satisfy;
