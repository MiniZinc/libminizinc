% RUNS ON minizinc
% RUNS ON mzn_fd
% RUNS ON mzn_mer_fd
% RUNS ON mzn_mer_lazyfd
var 1..3: i;
var bool: y;
constraint y = [not(y), y, not(y)][i];

solve satisfy;

output [
   "i = ", show(i), ";\n",
   "y = ", show(y), ";\n"
];
