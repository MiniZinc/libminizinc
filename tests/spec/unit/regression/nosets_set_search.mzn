/***
!Test
solvers: [gecode, chuffed]
expected:
- !Result
  solution: !Solution
    x: !Range 1..3
***/

include "nosets.mzn";

var set of 1..3: x;
solve :: set_search([x], input_order, indomain_min) satisfy;
