% RUNS ON mzn_mer_std_fd
% RUNS ON mzn_mer_fd
% RUNS ON minizinc_cpx

% A test for value_precede_chain for integer variables.

include "value_precede_chain.mzn";

array[1..3] of var 1..4: x;

constraint value_precede_chain([3, 2, 1], [3, 2, 1]);
constraint value_precede_chain([4, 3, 2], x);
solve satisfy;
output ["x = array1d(1..3, ", show(x), ");\n"];
