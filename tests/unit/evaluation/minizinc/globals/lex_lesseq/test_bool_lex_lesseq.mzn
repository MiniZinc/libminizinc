% RUNS ON mzn_mer_fd
% RUNS ON mzn_mer_lazyfd

include "lex_lesseq.mzn";

array[1..2] of var bool: x;
array[1..2] of var bool: y;

constraint lex_lesseq(x, y);

solve satisfy;

output [
	"x = ", show(x), "\n",
	"y = ", show(y), "\n"
];
