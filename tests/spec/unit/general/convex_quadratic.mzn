/***
!Test
solvers: [xpress]
expected: !Result
  solution: !Solution
    objective: !Approx 5.656871
***/

% Xpress convex quadratic constraint test: x^2 + y^2 <= 16 (a circle).
% maximize x + y => x = y = 2*sqrt(2), analytic optimum 4*sqrt(2) ~= 5.656854. Xpress solves
% this through the quadratic path (x*x/y*y added via XPRSaddqmatrix) and returns ~5.656871
% (the nonlinear solve is accurate to ~1e-5, hence !Approx against the solver's value).
% Regression guard for the diagonal-term coefficient: x*x must use qmatrix coefficient 1.0,
% not 0.5. With the old 0.5 bug the constraint became x^2+y^2<=32 and the objective was 8.0.
var 0.0..5.0: x;
var 0.0..5.0: y;

constraint x*x + y*y <= 16.0;
constraint x + y <= 10.0;

var float: objective = x + y;

solve maximize objective;

output ["x = ", show(x), ", y = ", show(y), "\n"];
