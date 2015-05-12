% testing the F-distribution for both float and integer degrees of freedom
var float: x;
var float: y;
float: myFloat1 = fdistribution(100, 100); % F-distribution with int degrees of freedom
float: myFloat2;
myFloat2 = cauchy(1.0, 1.0); % F-distribution with float degrees of freedom
constraint x + y < 10*myFloat1;
constraint x + y > myFloat2;

solve satisfy;
output ["x = ", show(x), "\ny = ", show(y)]