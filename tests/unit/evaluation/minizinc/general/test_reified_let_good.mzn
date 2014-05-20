% RUNS ON mzn20_fd
% RUNS ON mzn20_mip
var 1..10: x;
var bool: b;
constraint b <-> ( let { var 1..10: y = x + 1 } in x = y );
solve satisfy;
output [
   "b = ", show(b), ";\n",
   "x = ", show(x), ";\n"
];
