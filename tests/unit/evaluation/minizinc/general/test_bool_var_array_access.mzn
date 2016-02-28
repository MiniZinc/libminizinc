% RUNS ON mzn20_fd
% RUNS ON mzn-fzn_fd
% RUNS ON mzn20_mip
var 1..3: i;
var bool: y;
constraint y = [not(y), y, not(y)][i];

solve satisfy;

output [
   "i = ", show(i), ";\n",
   "y = ", show(y), ";\n"
];
