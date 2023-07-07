/***
!Test
solvers: [gecode, chuffed]
options:
  restart: "constant"
  restart-base: 100
expected: !Result
  status: OPTIMAL_SOLUTION
  solution: !Solution
    x: 3
***/
include "experimental/on_restart.mzn";
var -1..5: x;

predicate foo() =
	(status() != START) -> (x > 2 -> complete());

solve ::on_restart("foo") ::int_search([x], input_order, indomain_min)  maximize abs(x);
