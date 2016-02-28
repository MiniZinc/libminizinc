% RUNS ON mzn20_fd
% RUNS ON mzn-fzn_fd
% RUNS ON mzn20_fd_linear
% RUNS ON mzn20_mip
array [1..3] of var set of 1..5: a = [{1}, {2, 3}, {4, 5}];
solve satisfy;
output [ "a = ", show(a), "\n" ];
