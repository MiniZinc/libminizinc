% RUNS ON mzn_mer_fd
% RUNS ON mzn_mer_fdlp
% RUNS ON mzn_mer_lp
% RUNS ON mzn_mer_lazyfd
% RUNS ON minizinc_cpx

include "global_cardinality.mzn";

% The example from the GCC.
%
constraint global_cardinality([3, 3, 8, 6], [3, 5, 6], [2, 0, 1]);

array[1..9] of var -100..100: gcc_x = [6, 7, _, 8, _, 9, _, 8, 6];
array[1..4] of var -100..100: gcc_c = [5, 1, 2, _];

constraint global_cardinality(gcc_x, [6, 7, 8, 9], gcc_c);

solve satisfy;

output [
   "gcc_c = array1d(1..4, ", show(gcc_c), ");\n",
   "gcc_x = array1d(1..9, ", show(gcc_x), ");\n"
];
