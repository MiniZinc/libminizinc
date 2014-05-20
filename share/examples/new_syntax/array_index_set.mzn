% Array index sets can be arbitrary set valued expressions, as long as
% they evaluate to a contiguous range
set of int: S = 1..3;
set of int: T = 3..7;
array[3..6] of var int: x;

% The following was illegal in MiniZinc 1.6:
array[S union (T intersect index_set(x))] of var 0..3: y;

solve satisfy;
output [show(y)];
