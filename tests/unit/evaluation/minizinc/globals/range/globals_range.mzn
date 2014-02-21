% RUNS ON mzn_mer_fd
% RUNS ON mzn_mer_fdlp
% RUNS ON minizinc_cpx

include "range.mzn";

%-----------------------------------------------------------------------------%
% range
%-----------------------------------------------------------------------------%
array[1..3] of var int: range_avi1;

constraint range(range_avi1, {1, 2, 3}, {7, 8, 9});

solve satisfy;

output ["range_avi1 = array1d(1..3, ", show(range_avi1), ");\n"];
