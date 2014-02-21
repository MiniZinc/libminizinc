% RUNS ON minizinc
% RUNS ON mzn_fd
% RUNS ON mzn_mer_fd
% RUNS ON mzn_cd_fd
array [1..3] of var set of 1..5: a = [{1}, {2, 3}, {4, 5}];
solve satisfy;
output [ "a = ", show(a), "\n" ];
