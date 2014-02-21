% RUNS ON mzn_mer_fd
% RUNS ON mzn_mer_lp
% RUNS ON minizinc_cpx

include "global_cardinality_low_up_closed.mzn";

array[1..4] of var 0..8: x;

constraint
     global_cardinality_low_up_closed(
         %[3, 3, 5, 6],
         x,
         [3, 5, 6],
         [2, 1, 1],
         [3, 2, 1]);

solve satisfy;

output ["x = array1d(1..4, ", show(x), ");\n"];
