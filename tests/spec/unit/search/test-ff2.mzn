/***
--- !Test
solvers: [gecode, chuffed]
expected: !Result
  solution: !Solution
    x: 5
    y: 1
***/

var 1..8: x;
var 1..10: y;
var 1..10: z;
var 1..10: w;    
constraint x = 1 -> x < 1;
constraint x > 1 -> y < 5;
constraint x > 1 -> z < 5;
constraint x > 1 -> w < 5;
constraint y = 1 -> x > 4;

solve :: int_search([x,y,z,w],first_fail,indomain_min,complete)
                satisfy;
