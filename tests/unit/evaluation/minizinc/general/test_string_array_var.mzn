% RUNS ON mzn20_fd
% RUNS ON mzn20_fd_linear
% RUNS ON mzn20_mip
array [1..2] of string: s = ["All", " is"];
solve satisfy;
output s ++ [" well\n"];
