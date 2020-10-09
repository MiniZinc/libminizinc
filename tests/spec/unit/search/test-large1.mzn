/***
--- !Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    x: 5
    y: 8
--- !Test
solvers: [chuffed]
expected: !Result
  solution: !Solution
    x: 6
    y: 7
***/

var 1..10: x;
var 2..10: y;
constraint 7 < y \/ y > 6;
constraint x + y <= 13;

solve :: int_search([x,y],largest,indomain_split,complete)
                maximize x + y;
