% testing the binomial distribution for both float and integer means
var int: x;
var int: y;
int: myInt1 = binomial(10,0.5);
int: myInt2;
myInt2 = binomial(2,0.3);
constraint x + y < myInt1;
constraint x + y > myInt2;

solve satisfy;
output ["x = ", show(x), "\ny = ", show(y)];