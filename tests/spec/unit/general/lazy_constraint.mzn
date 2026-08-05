/***
!Test
solvers: [xpress]
expected: !Result
  solution: !Solution
    x: 10
    y: 0
***/

% Xpress lazy constraint (delayed row) test.
% The MIP_lazy annotation marks a linear constraint as delayed: Xpress keeps it out of the
% active matrix (via XPRSloaddelayedrows) and only reinserts it when an integer-feasible
% solution violates it. The optimum must equal the one obtained with the row enforced
% normally. Here 3*x + 2*y <= 30 is binding: without it, maximize 3*x + y gives x=10, y=1
% (obj 31); with it, the best is x=10, y=0 (obj 30).

var 0..10: x;
var 0..10: y;

constraint x + y <= 13;
constraint 2*x + y <= 21;

constraint (3*x + 2*y <= 30) :: MIP_lazy;

solve maximize 3*x + y;

output ["x = ", show(x), ", y = ", show(y), "\n"];