var int: a;
var int: b;

constraint 200*a + 150 * b <= 4000;
constraint 100*a + 250 * b >= 0;

solve satisfy;

output ["a = ",show(a), " b = ", show(b), "\n"];
%output ["a = \(a);\n"];
