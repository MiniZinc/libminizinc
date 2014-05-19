% Let expressions can now contain constraint items. Items can be separated
% with a semicolon instead of a comma.

var int: x = let {
  var 0..3: x;
  var int: y;
  constraint y = 10-x;
} in 3*y;

solve satisfy;
output [show(x)];
