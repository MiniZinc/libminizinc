% RUNS ON minizinc
% RUNS ON mzn_fd
% RUNS ON mzn_mer_fd
array [1..4] of var 1..10: xs;
var 0..1000: p = product(xs);
solve satisfy;
output [
    "p = ", show(p), ";\n",
    "xs = ", show(xs), ";\n"
];
