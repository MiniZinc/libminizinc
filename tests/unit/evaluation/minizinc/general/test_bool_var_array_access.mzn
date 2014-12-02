% RUNS ON mzn20_fd
% RUNS ON mzn20_fd_linear
var 1..3: i;
var bool: y;
constraint y = [not(y), y, not(y)][i];

solve satisfy;

output [
   "i = ", show(i), ";\n",
   "y = ", show(y), ";\n"
];
