% RUNS ON mzn_mer_fd
% RUNS ON mzn_mer_std_fd
% RUNS ON minizinc_cpx

include "roots.mzn";

array[0..2] of var 0..1000: x;

constraint roots(x, {0, 1, 2}, {5});

solve satisfy;

output [
   "x = array1d(0..2, ", show(x), ");\n"
];
