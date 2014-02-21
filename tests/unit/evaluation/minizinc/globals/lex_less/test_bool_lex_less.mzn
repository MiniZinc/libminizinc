% RUNS ON mzn_mer_fd
% RUNS ON mzn_mer_lazyfd
% RUNS ON minizinc_cpx

include "lex_less.mzn";

array[1..2] of var bool: x;
array[1..2] of var bool: y;

constraint lex_less(x, y);

solve satisfy;

output [
	"x = array1d(1..2, ", show(x), ");\n",
	"y = array1d(1..2, ", show(y), ");\n"
];
