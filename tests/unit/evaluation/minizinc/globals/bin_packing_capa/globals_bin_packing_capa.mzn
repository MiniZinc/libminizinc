% RUNS ON mzn_mer_fd
% RUNS ON minizinc_cpx

include "bin_packing_capa.mzn";

% The example from the GCC.
%
constraint bin_packing_capa([4, 3, 5, 3, 3], [3, 1, 3], [4, 3, 1]);

array[1..4] of var 1..4: bins;

constraint bin_packing_capa([1, 2, 1, 3], bins, [3, 2, 1, 1]);

solve satisfy;

output ["bins = array1d(1..4, ", show(bins), ");\n"];
