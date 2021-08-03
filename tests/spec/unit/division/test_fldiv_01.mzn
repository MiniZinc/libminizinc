/***
!Test
solvers: [gurobi, scip]
expected:
- !Result
  solution: !Solution
    x: 0.3
    y: 2.0
***/

var 0.3..2.0: x; 
var 1.5..2.0: y; 


solve minimize x / y;

