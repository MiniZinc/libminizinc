% RUNS ON mzn_mer_fd

array [1..3] of var 1..3: a;
var int: x = min(a);

solve maximize x;
output [
    "a = ", show(a), ";\n",
    "x = ", show(x), ";\n"
];
