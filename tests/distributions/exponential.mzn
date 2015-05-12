% testing the exponential distribution for both float and integer lambdas
var float: x;
var float: y;
float: myFloat1 = exponential(0.5); % exponential distribution with float lambda
float: myFloat2;
myFloat2 = exponential(9); % exponential distribution with int lambda
constraint x + y < myFloat1;
constraint x + y > myFloat2;

solve satisfy;
output ["x = ", show(x), "\ny = ", show(y)];