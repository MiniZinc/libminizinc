% RUNS ON mzn20_fd
% RUNS ON mzn20_fd_linear
array [1..4] of var 1..10: xs;
var 0..1000: p = product(xs);
solve satisfy;
output [
    "p = ", show(p), ";\n",
    "xs = ", show(xs), ";\n"
];
