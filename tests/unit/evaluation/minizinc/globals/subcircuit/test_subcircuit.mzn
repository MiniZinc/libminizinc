% RUNS ON mzn_mer_fd

include "subcircuit.mzn";

array[1..4] of var int: x;

constraint subcircuit(x);

solve satisfy;

output ["x = array1d(1..4, ", show(x), ");\n"];
