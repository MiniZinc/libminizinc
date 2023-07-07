/***
!Test
solvers: [gecode, chuffed]
options:
  restart: "constant"
  restart-base: 100
expected: !Result
  status: OPTIMAL_SOLUTION
  solution: !Solution
    x: 4
    y: 3
***/
include "experimental/on_restart.mzn";
var -1..4: x;
var int: y;

predicate foo() =
	if status() = START then y = 0 else y = last_val(y) + 1 endif;

solve ::on_restart("foo") ::int_search([x], input_order, indomain_min)  maximize abs(x);
