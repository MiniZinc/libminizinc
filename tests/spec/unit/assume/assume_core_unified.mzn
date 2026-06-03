/***
!Test
solvers: [chuffed]
type: core
expected: !Result
  status: UNSATISFIABLE
  core: ["p"]
***/

% The assumption variable ``p`` is unified away during optimisation (``p = q``), so the final
% FlatZinc references the surviving variable. The core-reverse map is updated across the
% unification, so the core is still reported as the original assumption ``p`` rather than the
% surviving variable's internal name.
include "experimental/assume.mzn";
var bool: p;
var bool: q;
var 1..10: x;
constraint p = q;
constraint assume([p]);
constraint q -> (x > 8);
constraint x < 3;
solve satisfy;
