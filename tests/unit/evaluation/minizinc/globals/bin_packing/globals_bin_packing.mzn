% RUNS ON mzn20_fd
% RUNS ON mzn-fzn_fd
include "bin_packing.mzn";

% The example from the GCC.
%
constraint bin_packing(5, [3, 1, 3], [4, 3, 1]);

array[1..6] of var 1..5: bins;

constraint bin_packing(3, bins, [1, 1, 1, 2, 3, 3]);

solve satisfy;

output ["bins = array1d(1..6, ", show(bins), ");\n"];
