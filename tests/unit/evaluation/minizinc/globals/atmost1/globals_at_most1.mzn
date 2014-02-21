% RUNS ON mzn_fd
% RUNS ON mzn_mer_fd
% RUNS ON mzn_mer_fdlp
% RUNS ON minizinc_cpx

include "at_most1.mzn";

array[1..3] of var set of 1..3: s;
constraint at_most1(s);
constraint at_most1([{5,8}, {5}, {5, 6, 7}, {1, 4}]);
solve satisfy;

output ["s = array1d(1..3, ", show(s), ");\n"];

