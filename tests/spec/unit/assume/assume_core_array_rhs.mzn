/***
!Test
solvers: [chuffed]
type: core
expected: !Result
  status: UNSATISFIABLE
  core: ["x < 3", "x > 8"]
***/

% The assumptions are passed as an identifier bound to an array literal. The individual
% element expressions are still recovered from the array's right-hand side, so the core is
% reported in terms of those expressions.
include "experimental/assume.mzn";
var 1..10: x;
array [int] of var bool: asmp = [x < 3, x > 8];
constraint assume(asmp);
solve satisfy;
