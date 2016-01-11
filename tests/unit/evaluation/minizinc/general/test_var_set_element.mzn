% RUNS ON mzn20_fd
% RUNS ON mzn20_mip
array [1..3] of var set of 1..5: a;
var 2..10: x;
var set of 1..10: s = a[x];
solve satisfy;
output [
    "a = ", show(a), ";\n",
    "s = ", show(s), ";\n",
    "x = ", show(x), ";\n"
];
