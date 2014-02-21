% RUNS ON minizinc
% RUNS ON mzn_fd
% RUNS ON mzn_mer_fd
% RUNS ON mzn_cd_fd
array [1..2] of string: s = ["All", " is"];
solve satisfy;
output s ++ [" well\n"];
