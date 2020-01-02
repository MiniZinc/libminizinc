/***
--- !Test
solvers: [gecode, chuffed]
expected: !Result
  solution: !Solution
    x: 9
    y: 2
***/

var 1..10: x;
var 2..10: y;
constraint x = 1 -> y > 10;
constraint x > 1 -> x > 4;
constraint x + y <= 11;

solve :: int_search([x,y],smallest,indomain_min,complete)
                maximize x + y;
