% testing Weibull distribution for float and int shape parameters
% weibull(shape-param, scale-param)
var float: x;
var float: y;
float: myFloat1 = weibull(10, 6.1); % Weibull distribution with integer shape parameter
float: myFloat2;
myFloat2 = weibull(1.2, 3.3); % Weibull distribution with float shape parameter
constraint x + y < myFloat1;
constraint x + y > myFloat2;

solve satisfy;
output ["x = ", show(x), "\ny = ", show(y)];