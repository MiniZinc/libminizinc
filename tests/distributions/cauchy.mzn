% testing the cauchy distribution for both float and integer means
var float: x;
var float: y;
float: myFloat1 = cauchy(10.3, 1.8); % cauchy distribution with float mean
float: myFloat2;
myFloat2 = cauchy(3, 0.25); % cauchy distribution with int mean
constraint x + y < myFloat1;
constraint x + y > myFloat2;

solve satisfy;
output ["x = ", show(x), "\ny = ", show(y)];