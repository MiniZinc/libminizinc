/***
---
!Test
solvers: [gecode]
options:
  all_solutions: true
extra_files:
- param_file_nested_object.mpc
expected: !Result
  status: SATISFIED
  solution:
  - !Solution
    x: 1
***/

var 1..10: x;

solve :: int_search([x], input_order, indomain_min) satisfy;
