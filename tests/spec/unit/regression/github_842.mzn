/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
***/

par int: x = 1;
var 0..: y;

function array[int] of ann: dummy_search() =
[ int_search([x], input_order, indomain_min) ];

solve :: seq_search(
  dummy_search() ++
  [ int_search([y], input_order, indomain_min) ]
) satisfy;
