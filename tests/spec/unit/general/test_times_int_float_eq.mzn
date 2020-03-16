/***
!Test
solvers: [gurobi]
options:
  -D: QuadrFloat=true;QuadrInt=true
expected: !Result
  status: OPTIMAL_SOLUTION
  solution: !Solution
    objective: -5
    x: 5
    y: 6
    z: 30
    xf: !Approx 5.0
    yf: !Approx 6.0
***/

%% TESTING NON-CONVEX MIQCP

var {1, 3, 5}: x;

var 1..6: y;

var 0..30: z;

var 0.5..7.2: yf;
var 2.5..6.2: xf;

constraint x * y == z;
constraint x * yf == z;
constraint xf * yf == z;

solve minimize 5 * x - z;
