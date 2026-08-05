/***
!Test
solvers: [xpress]
expected: !Result
  solution: !Solution
    x: 5.0
    y: 5.0
    objective: !Approx 25.000061
***/

% Xpress bilinear equality test: z = x * y (a quadratic product handled via XPRSaddqmatrix).
% maximize z with x + y <= 10 => x = y = 5, z = 25. x and y sit at the linear vertex so they
% are exact; the objective (z) carries small nonlinear-solver noise (~1e-5), so it is checked
% with !Approx against the solver's returned value rather than the analytic 25.0.
var 0.0..10.0: x;
var 0.0..10.0: y;
var 0.0..100.0: z;

constraint z = x * y;
constraint x + y <= 10.0;

solve maximize z;

output ["x = ", show(x), ", y = ", show(y), ", z = ", show(z), "\n"];
