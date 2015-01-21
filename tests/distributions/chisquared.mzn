% testing the chi-squared distribution for both float and integer degrees of freedom
var float: x;
var float: y;
float: myFloat1 = chisquared(11.3); % chi-squared distribution with float degree of freedom
float: myFloat2;
myFloat2 = chisquared(1); % chi-squared distribution with int degree of freedom
constraint x + y < 10*myFloat1;
constraint x + y > myFloat2;

solve satisfy;
output ["x = ", show(x), "\ny = ", show(y)];