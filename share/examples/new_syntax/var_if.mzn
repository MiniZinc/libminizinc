% The if-then-else construct has been extended to support var bool conditions.

var bool: b;
var 1..3: y;
var 4..6: z;
var int: x = if b then y else z endif;
solve satisfy;
output [show(x)];
