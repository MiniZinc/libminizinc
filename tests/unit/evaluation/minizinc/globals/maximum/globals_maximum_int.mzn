% RUNS ON mzn_mer_fd
% RUNS ON mzn_mer_fdlp
% RUNS ON mzn_mer_lp
% RUNS ON mzn_mer_lazyfd
% RUNS ON minizinc_cpx

include "maximum.mzn";

%-----------------------------------------------------------------------------%
% maximum
%-----------------------------------------------------------------------------%

var 0..20: max_vi1;
var 0..20: max_vi2;

constraint maximum(max_vi1, [5, 3, 8, 9,       4]);
constraint maximum(9,       [5, 3, 8, max_vi2, 4]);

solve satisfy;

output [
    "max_vi1 = ", show(max_vi1), ";\n",
    "max_vi2 = ", show(max_vi2), ";\n"
];
