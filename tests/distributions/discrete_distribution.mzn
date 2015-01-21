% testing the poisson distribution for both float and integer means
var int: x;
var int: y;
int: myInt1 = discrete_distribution([0,0,0,2,4,5,0,9,6]); % the weights for each integer, starting from zero
int: myInt2;
myInt2 = discrete_distribution([6,7,4,2]); % the weights for each integer, starting from zero
constraint x + y < myInt1;
constraint x + y > myInt2;

solve satisfy;
output ["x = ", show(x), "\ny = ", show(y)];