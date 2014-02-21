% RUNS ON mzn_mer_std_fd
% RUNS ON mzn_mer_fd
% RUNS ON minizinc_cpx

include "value_precede_chain.mzn";

% A test for value_precede_chain for set variables.

array[1..2] of var set of 1..4: x;

constraint value_precede_chain([3, 2, 1], [{3}, {2}, {1}]);
constraint value_precede_chain([4, 3, 2], x);
solve satisfy;
output ["x = array1d(1..2, ", show(x), ");\n"];
