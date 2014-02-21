% RUNS ON minizinc
% RUNS ON mzn_fd
% RUNS ON mzn_mer_fd
% RUNS ON mzn_cd_fd
solve satisfy;

output ["Escaped single \'quotes' are fine.\n"];
