% RUNS ON mzn_mer_fd
% RUNS ON mzn_mer_fdlp
% RUNS ON mzn_mer_lp
% RUNS ON mzn_mer_lazyfd
% RUNS ON minizinc_cpx

include "increasing.mzn";

%-----------------------------------------------------------------------------%
% increasing
%-----------------------------------------------------------------------------%
array[1..5] of var -100..100:   inc_avi = [1, _, 3, _, 5];
array[1..5] of var bool:        inc_avb = [false, true, _, _, _];
%array[1..8] of var set of 1..3: inc_avsi; % XXX :: is_output

constraint increasing(inc_avi);
constraint increasing(inc_avb);
% XXX: comparisons not implemented for sets
%constraint increasing(inc_avsi);

solve satisfy;

output [
   "inc_avb = array1d(1..5, ", show(inc_avb), ");\n",
   "inc_avi = array1d(1..5, ", show(inc_avi), ");\n"
];
