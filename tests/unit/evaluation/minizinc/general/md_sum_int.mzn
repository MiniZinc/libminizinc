% RUNS ON minizinc_fd
% RUNS ON minizinc_cpx

% Test sum/1 for integers with multi-dimensional arrays.

int: a = sum([|1, 2, 3, 4, 5 | 6, 7, 8, 9, 10|]);

array[1..2,1..2] of var 1..10: vs;
var -100..100: b = sum(vs);

solve satisfy;

output [
   "% a = ", show(a), ";\n",
   "b = ", show(b), ";\n"
];
