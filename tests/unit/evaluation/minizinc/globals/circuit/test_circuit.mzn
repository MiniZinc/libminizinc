% RUNS ON mzn_mer_fd
% RUNS ON minizinc_cpx

include "circuit.mzn";

array[1..4] of var 1..4: c1;
array[2..5] of var -100..100: c2;

constraint circuit(c1);
constraint circuit(c2);

solve satisfy;

output [
   "c1 = array1d(1..4, ", show(c1), ");\n",
   "c2 = array1d(2..5, ", show(c2), ");\n"
];
