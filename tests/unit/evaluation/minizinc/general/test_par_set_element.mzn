% RUNS ON minizinc
% RUNS ON mzn_fd
% RUNS ON mzn_mer_fd
array [1..3] of set of int: a = [{1}, {1, 2}, {1, 2, 3}];
var 1..2: x;
var set of 1..10: s = a[x];
solve satisfy;
output [
   "a = ", show(a), ";\n",
   "s = ", show(s), ";\n",
   "x = ", show(x), ";\n"
];
