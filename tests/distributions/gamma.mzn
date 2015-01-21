% testing gamma distribution for float and int aplha
var float: x;
var float: y;
float: myFloat1 = gamma(10, 1.0); % gamma distribution with integer alpha
float: myFloat2;
myFloat2 = gamma(5.0, 0.5); % gamma distribution with float alpha
constraint x + y < myFloat1;
constraint x + y > myFloat2;

solve satisfy;
output ["x = ", show(x), "\ny = ", show(y)];