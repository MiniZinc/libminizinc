% RUNS ON mzn_mer_fd
% RUNS ON mzn_mer_std_fd
% RUNS ON minizinc_cpx

include "roots.mzn";

var set of 1..5: s;
var set of 1..5: t;

constraint roots([1, 3, 1, 2, 3], s, t);

solve satisfy;

output [
   "s = ", show(s), ";\n",
   "t = ", show(t), ";\n"
];
