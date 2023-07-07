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
    y: !!set {1,2,4,8}
***/
include "experimental/on_restart.mzn";
var -1..4: x;
var set of 1..10: y;

predicate foo() =
	if status() = START then y = {1} else y = last_val(y) union {max(last_val(y))*2} endif;

solve ::on_restart("foo") ::int_search([x], input_order, indomain_min)  maximize abs(x);
