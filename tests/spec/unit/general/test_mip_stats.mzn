% Previously this output a statistic with an incorrect objective

var int: x;
var int: y;
constraint x = 3;
constraint y = 3;
solve maximize x + y;
