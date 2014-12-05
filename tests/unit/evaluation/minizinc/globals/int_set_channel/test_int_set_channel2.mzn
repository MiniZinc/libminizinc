% RUNS ON mzn20_fd
include "int_set_channel.mzn";

% Check that the domains of x and y are being restricted
% to the index sets.
%
array[1..9] of var int: x;
array[1..5] of var set of 1..20: y;

constraint int_set_channel(x, y);

solve satisfy;

output [
	"x = ", show(x), "\n",
	"y = ", show(y), "\n"
];
