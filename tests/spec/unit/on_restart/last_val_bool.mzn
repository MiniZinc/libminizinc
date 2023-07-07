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
    y: true
***/
include "experimental/on_restart.mzn";
var -1..4: x;
var bool: y;

predicate foo() =
  if status() = START then y = false else y = not last_val(y) endif;

solve ::on_restart("foo") ::int_search([x], input_order, indomain_min)  maximize abs(x);
