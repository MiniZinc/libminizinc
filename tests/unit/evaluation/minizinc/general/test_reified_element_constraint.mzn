% RUNS ON mzn20_fd
% RUNS ON mzn20_fd_linear
array [1..3] of var 1..10: a;
var bool: b;
var 4..6: i;
var 1..100: x;
constraint a[i] = x  <->  b;
solve satisfy;
output [
   "a = ", show(a), ";\n",
   "b = ", show(b), ";\n",
   "i = ", show(i), ";\n",
   "x = ", show(x), ";\n"
];
