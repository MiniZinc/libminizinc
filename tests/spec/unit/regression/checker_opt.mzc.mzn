int: _objective;

var 1..10: x;
var int: y ::add_to_output = x + _objective;

solve maximize y;
