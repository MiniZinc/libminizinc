/***
!Test
solvers: [gecode]
expected: !Error
  type: TypeError
***/
var int: x ::int_search([x], smallest, indomain_min);
