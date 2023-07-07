/***
!Test
solvers: [gecode]
options:
  restart: "constant"
  restart-base: 100
expected: !Result
  status: OPTIMAL_SOLUTION
  solution: !Solution
    x: 4
    y: 0.85
***/
include "experimental/on_restart.mzn";
var -1..4: x;
var -10.0..10.0: y;

predicate foo() =
	if status() = START then y = 0.1 else y = last_val(y) + 0.25 endif;

solve ::on_restart("foo") ::int_search([x], input_order, indomain_min)  maximize abs(x);
