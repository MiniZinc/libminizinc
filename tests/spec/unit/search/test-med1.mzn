/***
--- !Test
solvers: [gecode, chuffed]
expected: !Result
  solution: !Solution
    x: 5
    y: 6
***/

var 1..10: x;
var 2..10: y;
constraint x != y;

solve :: int_search([x,y],input_order,indomain_median,complete)
                satisfy;

