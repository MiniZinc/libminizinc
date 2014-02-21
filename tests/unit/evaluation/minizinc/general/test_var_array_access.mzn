% RUNS ON mzn_mer_fd

var 1..3: i;
var int: x;
var set of 1..3: y;
constraint x = [1, 2, 3][i];
constraint y = [{1}, {2}, {3}][i];

solve minimize(i);

output [
   "i = ", show(i), ";\n",
   "x = ", show(x), ";\n",
   "y = ", show(y), ";\n"
];
