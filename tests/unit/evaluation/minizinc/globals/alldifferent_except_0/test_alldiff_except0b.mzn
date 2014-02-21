% RUNS ON mzn_mer_fd
% RUNS ON minizinc_fd

include "alldifferent_except_0.mzn";

array[1..4] of var -1..2: vs;
constraint alldifferent_except_0(vs);
solve satisfy;
output ["vs = array1d(1..4, ", show(vs), ");"];
