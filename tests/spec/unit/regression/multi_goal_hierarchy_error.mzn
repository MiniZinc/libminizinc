/***
!Test
solvers: [cbc]
expected: !Result
  solution: !Solution
***/

% Used to cause internal error due to incorrect detection of multiple goal hierarchies.

var 1..10: x;
var 1..10: y;

solve :: int_search([x], input_order, indomain_min)
      :: int_search([y], input_order, indomain_max)
        satisfy;
