% RUNS ON mzn_mer_fd
% RUNS ON mzn_mer_fdlp
% RUNS ON mzn_mer_lp
% RUNS ON mzn_mer_lazyfd
% RUNS ON minizinc_cpx

include "count.mzn";

%-----------------------------------------------------------------------------%
% count
%-----------------------------------------------------------------------------%
var -100..100:   count_vi1;
var -100..100:   count_vi2;
var 8..20: count_vi3;
array[1..8] of var -100..100: count_avi1 = [7, 8, 8, 9, 9, 9, _, _];
array[1..8] of var -100..100: count_avi2 = [7, 8, 9, 3, 0, 0, 9, 9];
array[1..8] of var -100..100: count_avi3 = [7, 8, 8, 9, 9, 9, 0, 8];
array[1..8] of var -100..100: count_avi4;

constraint count(count_avi1, 8,         4);     % 1st arg unfixed -> [...]
constraint count(count_avi2, count_vi1, 3);     % 2nd arg unfixed -> 9
constraint count(count_avi3, 8, count_vi2);     % 3rd arg unfixed -> 3
constraint count(count_avi4, 0, count_vi3);     % 1st arg unfixed -> [0,0,...]

solve satisfy;

output [
    "count_avi1 = array1d(1..8, ", show(count_avi1), ");\n",
    "count_avi2 = array1d(1..8, ", show(count_avi2), ");\n",
    "count_avi3 = array1d(1..8, ", show(count_avi3), ");\n",
    "count_avi4 = array1d(1..8, ", show(count_avi4), ");\n",
    "count_vi1 = ", show(count_vi1), ";\n",
    "count_vi2 = ", show(count_vi2), ";\n",
    "count_vi3 = ", show(count_vi3), ";\n"
];
