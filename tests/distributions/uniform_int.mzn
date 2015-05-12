% testing uniform distribution for integers
var int: x;
var int: y;
int: myInt1 = uniform(7, 10);
int: myInt2;
myInt2 = uniform(2, 4);
constraint x + y < myInt1;
constraint x + y > myInt2;

solve satisfy;
output ["x = ", show(x), "\ny = ", show(y)];