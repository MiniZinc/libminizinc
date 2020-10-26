include "alldifferent.mzn";

array[1..15] of var bool: b;
array[1..4] of var 1..10: x;

constraint alldifferent(x) /\ sum(i in 1..4)(x[i]) = 9;

solve satisfy;

output ["b = \(b);\nx = \(x);\n"]
