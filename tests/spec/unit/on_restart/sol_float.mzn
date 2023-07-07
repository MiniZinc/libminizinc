/***
!Test
solvers: [gecode]
options:
  restart: "constant"
  restart-base: 100
expected: !Result
  status: OPTIMAL_SOLUTION
  solution: !Solution
    x: 3
    y: 2.0
***/
include "experimental/on_restart.mzn";

var 1..3: x;
var float: y;

predicate foo() =
	(status() != START) -> (y = sol(int2float(x)));

solve ::on_restart("foo") ::int_search([x], input_order, indomain_min)  maximize x;
