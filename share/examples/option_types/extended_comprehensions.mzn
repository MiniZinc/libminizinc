% Comprehensions can now range over var sets

array[int] of int: z = [2,3,5,7,11];
var set of 1..5: y;

% This used to be illegal in MiniZinc 1.6
var int: x = sum (i in y) (z[i]);

solve satisfy;
output [show(x)," ",show(y)];
