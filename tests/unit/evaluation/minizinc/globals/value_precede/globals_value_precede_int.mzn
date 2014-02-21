% RUNS ON mzn_mer_std_fd
% RUNS ON mzn_mer_fd
% RUNS ON mzn_mer_lazyfd
% RUNS ON minizinc_cpx

% A test for the integer version of value_precede/3.

include "value_precede.mzn";

array[1..3] of var 1..4: x;

constraint value_precede(19, 10, [1, 3, 5, 19, 3, 10]);
constraint value_precede(4, 3, [1, 2, 1, 2, 5]);	% s and t not in x.
constraint value_precede(4, 2, x) /\ x[3] = 3;

solve satisfy;

output ["x = array1d(1..3, ", show(x), ");\n"];
