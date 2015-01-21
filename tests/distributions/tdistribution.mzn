% testing the student's t-distribution for both float and integer sample size
var float: x;
var float: y;
float: myFloat1 = abs(tdistribution(1)); % t-distribution with int sample size
float: myFloat2;
myFloat2 = tdistribution(5.0); % t-distribution with float sample size
constraint x + y < myFloat1;
constraint x + y > myFloat2;

solve satisfy;
output ["x = ", show(x), "\ny = ", show(y)];