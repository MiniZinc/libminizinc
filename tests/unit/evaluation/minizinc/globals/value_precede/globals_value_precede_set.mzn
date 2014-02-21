% RUNS ON mzn_mer_std_fd
% RUNS ON mzn_mer_fd
% RUNS ON minizinc_cpx

% A test for the set version of value_precede/3.

include "value_precede.mzn";

array[1..3] of var set of 1..3: x;

constraint value_precede(4, 10, [{4}, {10}, {}]);
constraint value_precede(6, 4, [{}, {1}, {2, 3}]);
constraint value_precede(3, 2, x);
solve satisfy;
output ["x = array(1..3, ", show(x), ");\n"];

