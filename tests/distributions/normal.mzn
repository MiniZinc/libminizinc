% testing the normal distribution for both float and integer means
var float: x;
var float: y;
float: myFloat1 = normal(10.0, 1.0); % normal distribution with float mean
float: myFloat2;
myFloat2 = normal(5, 0.15); % normal distribution with int mean
constraint x + y < myFloat1;
constraint x + y > myFloat2;

solve satisfy;
output ["x = ", show(x), "\ny = ", show(y)];