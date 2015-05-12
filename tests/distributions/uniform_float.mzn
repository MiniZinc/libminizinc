% testing uniform distribution for floats
var float: x;
var float: y;
float: myFloat1 = uniform(7.8, 10.1);
float: myFloat2;
myFloat2 = uniform(2.0, 4.0);
constraint x + y < myFloat1;
constraint x + y > myFloat2;

solve satisfy;
output ["x = ", show(x), "\ny = ", show(y)];