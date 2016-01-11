% RUNS ON mzn20_fd
% RUNS ON mzn20_fd_linear
% RUNS ON mzn20_mip
solve satisfy;

output ["Escaped single \'quotes' are fine.\n"];
