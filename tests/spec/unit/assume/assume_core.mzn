/***
!Test
solvers: [chuffed]
type: core
expected: !Result
  status: UNSATISFIABLE
  core: ["x < 3", "x > 8"]
***/

% The two assumptions are jointly unsatisfiable (and neither alone is), so a solver that
% supports assumptions must report both of them as the unsatisfiable core, reported in terms
% of the original expressions.
include "experimental/assume.mzn";
var 1..10: x;
constraint assume([x < 3, x > 8]);
solve satisfy;
