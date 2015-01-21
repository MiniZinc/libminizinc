% testing the Bernoulli distribution for both float and integer means
bool: myBool1;
bool: myBool2 = bernoulli(0.3);
var int: x;
var int: y;
myBool1 = bernoulli(0.7);

constraint 
  myBool1 -> x + y = 0;
constraint
  myBool2 -> x > 3;
  
solve satisfy;
output ["x = ", show(x), "\ny = ", show(y)];