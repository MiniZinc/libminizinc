/***
!Test
solvers: [gecode, chuffed]
options:
  restart: "constant"
  restart-base: 100
expected: !Result
  status: OPTIMAL_SOLUTION
  solution: !Solution
    x: !!set {}
    y: !!set {1}
***/
include "experimental/on_restart.mzn";
var set of 1..3: x;
var set of 1..100: y;

predicate foo() =
	if status() = START then y = {} else y = sol(x) endif;

solve ::on_restart("foo") ::set_search([x], input_order, indomain_min) minimize sum(i in x)(i);
