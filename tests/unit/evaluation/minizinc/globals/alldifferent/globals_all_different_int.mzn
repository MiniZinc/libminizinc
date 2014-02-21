% RUNS ON mzn_mer_fd
% RUNS ON mzn_mer_std_fd
% RUNS ON mzn_mer_lp
% RUNS ON mzn_mer_lazyfd
% RUNS ON minizinc_cpx

% Test all_different for var int.

include "all_different.mzn";

array[5..9] of var 1..5:  alldiff_avi1 =
	array1d(5..9, [1, _, 3, _, 5]);

constraint all_different(alldiff_avi1);
constraint alldiff_avi1[6] < alldiff_avi1[8];

solve satisfy;

output ["alldiff_avi1 = array1d(5..9, ", show(alldiff_avi1), ");\n"];
