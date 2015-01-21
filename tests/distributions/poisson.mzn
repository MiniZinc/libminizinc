% testing the poisson distribution for both float and integer means
var int: x;
var int: y;
int: myInt1 = poisson(7); % poisson distribution with int mean
int: myInt2;
myInt2 = poisson(2.2); % poisson distribution with float mean
constraint x + y < myInt1;
constraint x + y > myInt2;

solve satisfy;
output ["x = ", show(x), "\ny = ", show(y)];