/***
!Test
solvers: [gurobi, scip]
expected:
- !Result
  solution: !Solution
    x: 2.0
    y: 1.5
    z: 1.5
***/

var 0.3..2.0: x; 
var 1.5..2.0: y; 
var 1.5..2.0: z; 


solve maximize abs( x / y / z );

