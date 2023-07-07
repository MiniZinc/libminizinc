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
    is_even: false
    y: true
***/
include "experimental/on_restart.mzn";
var -1..3: x;
var bool: is_even;
var bool: y;

constraint is_even = (abs(x) mod 2 = 0);

predicate foo() =
  (status() != START) -> (y = sol(is_even));

solve ::on_restart("foo") ::int_search([x], input_order, indomain_min)  maximize abs(x);
