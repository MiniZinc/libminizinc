/***
!Test
solvers: [chuffed]
type: core
expected: !Result
  status: UNSATISFIABLE
  core: ["x+y < 5", "x+y > 15"]
***/

% Same as assume_core, but the assumptions are over a linear expression (so they are
% half-reified). The core is still reported in terms of the original expressions.
include "experimental/assume.mzn";
var 1..10: x;
var 1..10: y;
constraint assume([x + y < 5, x + y > 15]);
solve satisfy;
