% RUNS ON mzn_mer_fd
% RUNS ON mzn_mer_fdlp
% RUNS ON mzn_mer_lp
% RUNS ON mzn_mer_lazyfd
% RUNS ON minizinc_cpx

include "inverse.mzn";

%-----------------------------------------------------------------------------%
% inverse
%-----------------------------------------------------------------------------%
array[1..4] of var -100..100: inv_f1 = [5, 7, _, _];
array[5..8] of var -100..100: inv_f2 = array1d(5..8, [1, _, 2, _]);

constraint inverse(inv_f1, inv_f2);

solve satisfy;

output [
    "inv_f1 = array1d(1..4, ", show(inv_f1), ");\n",
    "inv_f2 = array1d(5..8, ", show(inv_f2), ");\n"
];
