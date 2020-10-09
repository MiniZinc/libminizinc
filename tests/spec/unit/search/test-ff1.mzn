/***
!Test
solvers: [gecode, chuffed]
expected: !Result
  solution: !Solution
    x: 3
    y: 8
***/

var 1..8: x;
var 1..10: y;
constraint x > 1 -> y > 7;
constraint x = 1 -> y < 3;
constraint x + y <= 11;

solve :: int_search([y,x],first_fail,indomain_min,complete)
                maximize x + y;
