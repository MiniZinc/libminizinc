int:       d = -1;
var 0..10: x;
var -3..6: y;
var 3..8:  z;
constraint 3*x - y + x * z <= 19 + d * (x + y + z) - 4*d;
solve satisfy;
