% RUNS ON mzn20_fd
% RUNS ON mzn-fzn_fd
include "minimum.mzn";
include "maximum.mzn";

%-----------------------------------------------------------------------------%
% minimum
%-----------------------------------------------------------------------------%
var 0..20: min_vi1;
var 0..20: min_vi2;

constraint minimum(min_vi1, [5, 3, 8, 0,       4]);
constraint minimum(0,       [5, 3, 8, min_vi2, 4]);

solve satisfy;

output [
    "min_vi1 = ", show(min_vi1), ";\n",
    "min_vi2 = ", show(min_vi2), ";\n"
];
